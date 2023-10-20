
msa: msa.cpp
	apt install -y libmodbus-dev libsimdjson-dev
	g++ $^ -O3 -I/usr/include/modbus/ -I/usr/include/ -std=c++20 -lpthread -lmodbus -lsimdjson -o $@

test: msa
	/usr/bin/time -f"Executed:%es\nRAM:%MKb\nSystem:%Ss" ./msa --assetid 1 --config ./config-msa.json
	./mca