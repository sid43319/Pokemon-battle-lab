# Convenience commands; CMake owns the actual build configuration.
.DEFAULT_GOAL := build

.PHONY: configure build test run

configure:
	cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug

build: configure
	cmake --build build/debug

test: build
	ctest --test-dir build/debug --output-on-failure

run: build
	./build/debug/battle_cli
