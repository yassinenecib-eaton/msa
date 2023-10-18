#include <errno.h>
#include <modbus.h> //modbus protocol

#include <cstdio>   //printf
#include <iostream> //cout
#include "simdjson.h"
using namespace simdjson;

int main(int argc, char** argv) { // Write to Modbus TCP register

  char ip[16] = "127.0.0.1"; //"172.16.1.225";

  modbus_t* ctx; int rc = 0;
  ctx = modbus_new_tcp(ip, 1502); //10.130.129.18
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

  ondemand::parser parser;
  auto json = padded_string::load("../../../config-set.json");
  ondemand::document d = parser.iterate(json);  //std::cout << uint64_t(d["search_metadata"]["count"]) << " results." << std::endl;

  for (auto point : d["points"]) {
    uint16_t data[4] = { 0,0,0,0 };
    uint64_t w = point["type"].get_uint64();
    uint64_t addr = (int)point["addr"].get_uint64();
    uint64_t size = (int)point["size"].get_uint64();
    std::cout << point["name"].get_string() << std::endl;
    switch (w) {
    case 1: { int32_t t1 = point["value"].get_int64();
      if (size == 1) rc = modbus_write_register(ctx, addr, t1);
      else { MODBUS_SET_INT32_TO_INT16(data, 0, t1); rc = modbus_write_registers(ctx, addr, size, data); }
      break; }
    case 2: { uint32_t t2 = point["value"].get_uint64();
      if (size == 1) rc = modbus_write_register(ctx, addr, t2);
      else { MODBUS_SET_INT32_TO_INT16(data, 0, t2); rc = modbus_write_registers(ctx, addr, size, data); }
      break; }
    case 3: { double t3 = point["value"].get_double();
      modbus_set_float_dcba((float)t3, data); rc = modbus_write_registers(ctx, addr, size, data);
      break; }
    }

    if (rc != size) {
      fprintf(stderr, "Failed to write to Modbus TCP register: %s\n", modbus_strerror(errno));
      modbus_close(ctx);
      modbus_free(ctx);
      return 1;
    }
  }

  modbus_close(ctx);
  modbus_free(ctx);
  return 0;
}
//*** Fin

void check(auto _ctx) {
  uint16_t reg[4] = { 0,0,0,0 };
  int rc = modbus_read_registers(_ctx, 80, 1, reg);

#if defined(_MSC_VER)
  float r = modbus_get_float_dcba(reg);
#elif defined(__GNUC__)
  float r = modbus_get_float_abcd(reg);
#endif
  printf("%f\n", r);
  return;
}

void t() {
  ondemand::parser p;
  auto cars_json = R"( [
  { "make": "Toyota", "model": "Camry",  "year": 2018, "tire_pressure": [ 40.1, 39.9, 37.7, 40.4 ] },
  { "make": "Kia",    "model": "Soul",   "year": 2012, "tire_pressure": [ 30.1, 31.0, 28.6, 28.7 ] },
  { "make": "Toyota", "model": "Tercel", "year": 1999, "tire_pressure": [ 29.8, 30.0, 30.2, 30.5 ] }
] )"_padded;

  // Iterating through an array of objects
  for (ondemand::object car : p.iterate(cars_json)) {
    // Accessing a field by name
    std::cout << "Make/Model: " << std::string_view(car["make"]) << "/" << std::string_view(car["model"]) << std::endl;

    // Casting a JSON element to an integer
    uint64_t year = car["year"];
    std::cout << "- This car is " << 2020 - year << "years old." << std::endl;

    // Iterating through an array of floats
    double total_tire_pressure = 0;
    for (double tire_pressure : car["tire_pressure"]) {
      total_tire_pressure += tire_pressure;
    }
    std::cout << "- Average tire pressure: " << (total_tire_pressure / 4) << std::endl;
  }
}