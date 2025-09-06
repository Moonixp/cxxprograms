CXX_SRC_FILES:=$(shell find src -iname *.cxx)
CC=g++
in_file=

all: build
	$(CC)  $(CXX_SRC_FILES) -o build/detectenc
	@echo "binary at: build/detectenc"

run:
	./build/detectenc $(in_file)

test:
	cd ./tests && ./run_tests.sh

build:
	mkdir -p build/

clean:
	mkdir -p build/
	rm -rf build/
