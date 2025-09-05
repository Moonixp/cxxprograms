BUILD_DIR=build
BINARIES_DIR=binaries
BIN_NAME=detectpessh

all: build
	g++ main.cxx -o $(BUILD_DIR)/$(BIN_NAME)
	
run:
	./$(BUILD_DIR)/$(BIN_NAME) $(BINARIES_DIR)/putty.exe

build:
	mkdir -p ${BUILD_DIR} 

clean: build
	rm -rf build/
