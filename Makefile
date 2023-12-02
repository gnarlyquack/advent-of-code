DEBUGGER := gdb

.PHONY: aoc2015
aoc2015: build/Makefile
	cmake --build build


build:
	mkdir build


build/Makefile: Makefile CMakeLists.txt | build
	cmake -B build .


.PHONY: run
run: build/Makefile
	cmake --build build --target run2015


.PHONY: debug
debug: build/Makefile
	cmake --build build --target debug2015


.PHONY: clean
clean:
	rm -rf build
