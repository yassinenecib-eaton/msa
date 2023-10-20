#include <errno.h>
#include <modbus.h> //modbus protocol

#include <chrono>   //chrono C++20
#include <cstdio>   //printf
#include <iostream> //cout
#include "simdjson.h"
using namespace simdjson;

int main(int argc, char** argv) { // Write to Modbus TCP register

  uint16_t i = 0, k = 0, assetid = 0;
  char f[128] = "../../../config-mca.json";
  char ip[16] = "0.0.0.0";
  int port = 0;

  while (i < argc) {
    if (strcmp(argv[i], "--assetid") == 0) { ++i; assetid = atoi(argv[i]); k++; }
    if (strcmp(argv[i], "--config") == 0) { ++i; if (strlen(argv[i]) < sizeof(f)) { strcpy(f, argv[i]); k++; } }
    ++i;
  }
  if ((argc - 1) / 2 != k || k == 0) { std::cout << "MSA - MODBUS Collector Agent by Guy Francoeur (c) 2023\nUsage : " << argv[0] << "\n\t{--runfor 00:30}\n\t{--id 1}\n\t{--assetid 1}\n\t{--config ./file.json}\n--runfor\tHourMinute the application will be running. It stops after HH:MM human time;\n--id    \tThe configuration ID to use;\n--assetid \tThe asset ID to use;\n--config\tThe json configuration file to use;\n\n"; return 0; }

  ondemand::parser parser;
  auto json = padded_string::load(f);
  ondemand::document d = parser.iterate(json);

  for (auto asset : d["assets"]) {
    if (asset["id"].get_uint64() == assetid) {
      std::string sIP = std::string{ std::string_view(asset["ip"]) };
      if (strlen(sIP.c_str()) < sizeof(ip)) { strcpy(ip, sIP.c_str()); }
      port = (int)asset["port"].get_uint64();
    }
  }
  auto start = std::chrono::steady_clock::now(); //steady is useful to compute durations (not impacted by clock adjustment)

  modbus_t* ctx; int rc = 0;
  ctx = modbus_new_tcp(ip, port);
  if (ctx == NULL) {
    fprintf(stderr, "Unable to create Modbus TCP context: %s\n", modbus_strerror(errno));
    return -1;
  }

  if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Unable to connect to Modbus TCP device: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
  }

  printf("%d\n", modbus_get_slave(ctx));  // check the slaveID ...
  modbus_set_slave(ctx, 1); // required

  for (auto asset : d["assets"]) {
    if (asset["id"].get_uint64() == assetid) {
      for (auto point : asset["points"]) {
        uint16_t data[4] = { 0,0,0,0 };
        uint64_t w = point["type"].get_uint64();
        uint64_t addr = (int)point["addr"].get_uint64();
        uint64_t size = (int)point["size"].get_uint64();
        std::cout << point["name"].get_string() << std::endl;
        switch (w) {
        case 1: { int32_t t1 = point["value"].get_int64();
          if (size == 1) rc = modbus_write_register(ctx, addr, t1);
          else {
            MODBUS_SET_INT32_TO_INT16(data, 0, t1);
            rc = modbus_write_registers(ctx, addr, size, data);
          }
          break; }
        case 2: { uint32_t t2 = point["value"].get_uint64();
          if (size == 1) rc = modbus_write_register(ctx, addr, t2);
          else {
            MODBUS_SET_INT32_TO_INT16(data, 0, t2);
            rc = modbus_write_registers(ctx, addr, size, data);
          }
          break; }
        case 3: { double t3 = point["value"].get_double();
#if defined(_MSC_VER)
          modbus_set_float_dcba((float)t3, data);
#elif defined(__GNUC__)
          modbus_set_float_abcd((float)t3, data);
#endif
          rc = modbus_write_registers(ctx, addr, size, data);
          break; }
        }

        if (rc != size) {
          fprintf(stderr, "Failed to write to Modbus TCP register: %s\n", modbus_strerror(errno));
          modbus_close(ctx);
          modbus_free(ctx);
          return 1;
        }
      }
    }
  }

  modbus_close(ctx);
  modbus_free(ctx);

  auto t3 = std::chrono::steady_clock::now() - start;
  std::cout << "Total human execution time : " << std::chrono::duration_cast<std::chrono::milliseconds>(t3).count() << " ms" << std::endl;
  return 0;
}
//*** Fin