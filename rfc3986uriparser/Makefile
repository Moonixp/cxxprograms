CXX_SRC:=main.cxx
CC:=g++

LIB_CXX_FILES := $(shell find lib/src/ -name '*.cxx')
TEST_CXX_FILES := $(shell find test/src/ -name '*.cxx')

CXXFLAGS= -Wall -std=c++23 -ggdb
CXX_TEST_FLAGS= -lgtest

TEST_BIN=build/uri_parser_test
MAIN_BIN=build/uri_parser

cmake-test-build:
	@cmake -S test/ -B test_build/
	@cmake --build test_build/

cmake-main-build:
	@cmake -S . -B build/
	@cmake --build build/

native-test-build: native-create-build
	$(CC) $(LIB_CXX_FILES)  $(TEST_CXX_FILES) $(CXXFLAGS) $(CXX_TEST_FLAGS) -o $(TEST_BIN)
	@echo -e "\n-------------------------------------------------------------------"
	@echo binary :./${TEST_BIN}

native-main: native-create-build
	$(CC) $(LIB_CXX_FILES) $(CXX_SRC) $(CXXFLAGS) -o $(MAIN_BIN) 
	@echo -e "\n-------------------------------------------------------------------"
	@echo binary :./$(MAIN_BIN)

native-build-debug: native-create-build
	$(CC) $(LIB_CXX_FILES) $(CXX_SRC) $(CXXFLAGS)  -o ${MAIN_BIN}_debug
	@echo -e "\n-------------------------------------------------------------------"
	@echo binary :./${MAIN_BIN}_debug

native-create-build:
	mkdir -p build/

clean:
	@mkdir -p build/ test_build/
	rm -rf build/ test_build/
	@echo "Cleaning done"

