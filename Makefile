
msa: msa.cpp
	apt install -y libmodbus-dev libsimdjson-dev
	g++ $^ -O3 -static -I/usr/include/modbus/ -I/usr/include/ -std=c++20 -lpthread -lmodbus -lsimdjson -o $@

test: msa
	/usr/bin/time -f"Executed:%es\nRAM:%MKb\nSystem:%Ss" ./msa --assetid 1 --config ./config-msa.json
	./msa

msa-arm64: msa.cpp
	apt install -y g++-12-aarch64-linux-gnu gcc-12-aarch64-linux-gnu
	apt install -y libmodbus-dev:arm64 libsimdjson-dev:arm64
	aarch64-linux-gnu-g++-12 $^ -O3 -I/usr/include/modbus/ -I/usr/include/ -L/usr/lib/aarch64-linux-gnu/ -std=c++20 -lpthread -lmodbus -lsimdjson -o $@

pack-arm64: msa-arm64
	rm -f msa-arm64.zip
	cp /usr/lib/aarch64-linux-gnu/libmodbus.so.5 .
	cp /usr/lib/aarch64-linux-gnu/libsimdjson.so.14 .
	zip msa-arm64.zip libmodbus.so.5 libsimdjson.so.14 msa-arm64 config-msa.json
	rm -f libmodbus.so.5 libsimdjson.so.14
