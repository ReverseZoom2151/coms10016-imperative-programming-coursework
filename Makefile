.PHONY: configure build test check format format-check coverage clean

configure:
	cmake -S . -B build

build: configure
	cmake --build build

test: build
	ctest --test-dir build --output-on-failure

check: test
	valgrind --leak-check=full --error-exitcode=1 ./build/binary-tests
	valgrind --leak-check=full --error-exitcode=1 ./build/list-tests
	valgrind --leak-check=full --error-exitcode=1 ./build/generic-list-tests
	valgrind --leak-check=full --error-exitcode=1 ./build/sketch-tests
	valgrind --leak-check=full --error-exitcode=1 ./build/robustness-tests

format:
	clang-format -i app/*.c include/*.h src/*.c test/*.c examples/*.c

format-check:
	clang-format --dry-run --Werror app/*.c include/*.h src/*.c test/*.c examples/*.c

coverage:
	cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug -DTOOLKIT_ENABLE_COVERAGE=ON
	cmake --build build-coverage
	ctest --test-dir build-coverage --output-on-failure
	gcov -o build-coverage/CMakeFiles/imperative_core.dir/src/binary.c.gcda src/binary.c
	gcov -o build-coverage/CMakeFiles/imperative_core.dir/src/generic_list.c.gcda src/generic_list.c
	gcov -o build-coverage/CMakeFiles/imperative_core.dir/src/list.c.gcda src/list.c
	gcov -o build-coverage/CMakeFiles/imperative_core.dir/src/sketch.c.gcda src/sketch.c

clean:
	cmake -E rm -rf build
