.PHONY: build run clean

build:
	cmake -S . -B build
	cmake --build build -j

run: build
	./build/c_fullstack_server --db-config ./server/db.conf

clean:
	cmake -E rm -rf build data
