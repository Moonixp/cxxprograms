BUILD_DIR=build
BINARIES_DIR=binaries
BIN_NAME=detectpessh
CXX_SRC=$(shell find src -iname "*.cxx")
file_in=

all: build
	g++ $(CXX_SRC) -o $(BUILD_DIR)/$(BIN_NAME)
	
run:
	./$(BUILD_DIR)/$(BIN_NAME) $(file_in) 

test:
	cd tests/ && ./run_tests.sh

build:
	mkdir -p ${BUILD_DIR} 

clean: build
	rm -rf build/
