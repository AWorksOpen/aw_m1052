all:
	cd 3rd/mbedtls/;mkdir -p build; cd build; cmake ../ && make
	mkdir -p build; cd build && cmake -DBUILD_DEVICE=1 -DBUILD_GATEWAY=1 ../ && make

arm:
	cd 3rd/mbedtls/;mkdir -p build; cd build; cmake -DARM_LINUX=1 ../ && make
	mkdir -p build; cd build && cmake -DARM_LINUX=1 ../ && make

clean:
	rm -rf 3rd/mbedtls/build
	rm -rf build bin vs2015 vs2012 vs2017

device:
	cd 3rd/mbedtls/;mkdir -p build; cd build; cmake ../ && make
	mkdir -p build; cd build && cmake -DBUILD_DEVICE=1 ../ && make

gateway:
	cd 3rd/mbedtls/;mkdir -p build; cd build; cmake ../ && make
	mkdir -p build; cd build && cmake -DBUILD_GATEWAY=1 ../ && make