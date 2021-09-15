###
# Makefile to build 'app_client' application.
#
# Authors:
#   Felipe Girardi 
#   Laura Corssac
#   Marlize Ramos
#   Renan Kummer
###

SRC_DIR = src
OBJ_DIR = out
BIN_DIR = bin
TEST_DIR = tests
HEADERS_DIR = $(SRC_DIR)/headers
COMMON_OBJ_DIR = ../common/$(OBJ_DIR)
INCLUDE_PATH = -I$(HEADERS_DIR)
LIBRARIES = -lpthread -lncurses

all: build-directories

###
# Main object files
###

ConcurrentCommandLine.o: $(SRC_DIR)/io/ConcurrentCommandLine.cpp
	g++ -o $(OBJ_DIR)/ConcurrentCommandLine.o -c $(SRC_DIR)/io/ConcurrentCommandLine.cpp $(INCLUDE_PATH)

###
# Test object files
###

ConcurrentCommandLineTest.o: $(TEST_DIR)/io/ConcurrentCommandLineTest.cpp
	g++ -o $(OBJ_DIR)/ConcurrentCommandLineTest.o -c $(TEST_DIR)/io/ConcurrentCommandLineTest.cpp $(INCLUDE_PATH)

###
# Test executables
###

ConcurrentCommandLineTest: ConcurrentCommandLine.o ConcurrentCommandLineTest.o
	g++ -o $(BIN_DIR)/ConcurrentCommandLineTest.out $(OBJ_DIR)/ConcurrentCommandLine.o $(OBJ_DIR)/ConcurrentCommandLineTest.o $(LIBRARIES)

###
# Scripts
###

build-directories:
	@[ ! -d $(OBJ_DIR) ] && mkdir $(OBJ_DIR) || true
	@[ ! -d $(BIN_DIR) ] && mkdir $(BIN_DIR) || true

check: build-directories ConcurrentCommandLineTest
	./$(BIN_DIR)/ConcurrentCommandLineTest.out

clean:
	@[ -d $(OBJ_DIR) ] && rm -r $(OBJ_DIR) || true
	@[ -d $(BIN_DIR) ] && rm -r $(BIN_DIR) || true
