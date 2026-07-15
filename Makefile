.PHONY: configure build test clean

configure:
	cmake -S . -B build

build: configure
	cmake --build build

test: build
	ctest --test-dir build --output-on-failure

clean:
	cmake -E rm -rf build

