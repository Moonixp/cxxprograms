CXX_SRC=main.cxx
CC=g++
in_file=

all: build
	$(CC) $(CXX_SRC)  -o build/detectenc
	@echo "binary at: build/detectenc"

run:
	./build/detectenc $(in_file)

build:
	mkdir -p build/

clean:
	mkdir -p build/
	rm -rf build/
