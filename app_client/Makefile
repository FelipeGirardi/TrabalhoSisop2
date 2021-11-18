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
INCLUDE_PATH = -I$(HEADERS_DIR) -I../common/include
LIBRARIES = -lpthread -lncurses -luuid
DEBUG_MODE = -g

all: build-directories ClientApp.o GlobalExceptionHandler.o GlobalSignalHandler.o CommandLineParser.o ConcurrentCommandLine.o Socket.o MessageWriter.o NotificationListener.o SessionManager.o
	g++ -std=c++11 -o $(BIN_DIR)/clientApp $(OBJ_DIR)/ClientApp.o $(COMMON_OBJ_DIR)/*.o \
	    $(OBJ_DIR)/GlobalExceptionHandler.o $(OBJ_DIR)/GlobalSignalHandler.o \
		$(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/ConcurrentCommandLine.o $(OBJ_DIR)/Socket.o \
		$(OBJ_DIR)/MessageWriter.o $(OBJ_DIR)/NotificationListener.o $(OBJ_DIR)/SessionManager.o \
		$(LIBRARIES)

###
# Main object files
###

ClientApp.o: $(SRC_DIR)/main/ClientApp.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/ClientApp.o -c $(SRC_DIR)/main/ClientApp.cpp $(INCLUDE_PATH)

GlobalExceptionHandler.o: $(SRC_DIR)/main/config/GlobalExceptionHandler.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/GlobalExceptionHandler.o -c $(SRC_DIR)/main/config/GlobalExceptionHandler.cpp $(INCLUDE_PATH)

GlobalSignalHandler.o: $(SRC_DIR)/main/config/GlobalSignalHandler.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/GlobalSignalHandler.o -c $(SRC_DIR)/main/config/GlobalSignalHandler.cpp $(INCLUDE_PATH)

CommandLineParser.o: $(SRC_DIR)/main/io/CommandLineParser.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/CommandLineParser.o -c $(SRC_DIR)/main/io/CommandLineParser.cpp $(INCLUDE_PATH)

ConcurrentCommandLine.o: $(SRC_DIR)/main/io/ConcurrentCommandLine.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/ConcurrentCommandLine.o -c $(SRC_DIR)/main/io/ConcurrentCommandLine.cpp $(INCLUDE_PATH)

Socket.o: $(SRC_DIR)/main/io/Socket.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/Socket.o -c $(SRC_DIR)/main/io/Socket.cpp $(INCLUDE_PATH)

MessageWriter.o: $(SRC_DIR)/main/service/MessageWriter.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/MessageWriter.o -c $(SRC_DIR)/main/service/MessageWriter.cpp $(INCLUDE_PATH)

NotificationListener.o: $(SRC_DIR)/main/service/NotificationListener.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/NotificationListener.o -c $(SRC_DIR)/main/service/NotificationListener.cpp $(INCLUDE_PATH)

SessionManager.o: $(SRC_DIR)/main/service/SessionManager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/SessionManager.o -c $(SRC_DIR)/main/service/SessionManager.cpp $(INCLUDE_PATH)

###
# Test object files
###

ConcurrentCommandLineTest.o: $(TEST_DIR)/io/ConcurrentCommandLineTest.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/ConcurrentCommandLineTest.o -c $(TEST_DIR)/io/ConcurrentCommandLineTest.cpp $(INCLUDE_PATH)

###
# Test executables
###

ConcurrentCommandLineTest: ConcurrentCommandLine.o ConcurrentCommandLineTest.o
	g++ -std=c++11 -o $(BIN_DIR)/ConcurrentCommandLineTest.out $(OBJ_DIR)/ConcurrentCommandLine.o $(OBJ_DIR)/ConcurrentCommandLineTest.o $(LIBRARIES)

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
