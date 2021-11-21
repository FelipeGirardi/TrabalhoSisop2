###
# Makefile to build 'app_frontend' application.
#
# Authors:
#   Felipe Girardi 
#   Laura Corssac
#   Marlize Ramos
#   Renan Kummer
###

##
# Constants
##
SRC_DIR = src
OBJ_DIR = out
BIN_DIR = bin
COMMON_OBJ_DIR = ../common/$(OBJ_DIR)

INCLUDE_PATH = -Isrc -I../common/include
LIBRARIES = -lpthread -luuid
DEBUG_MODE = -g


all: build-directories FrontendApp.o
	g++ -std=c++17 -o $(BIN_DIR)/frontendApp $(OBJ_DIR)/*.o $(COMMON_OBJ_DIR)/*.o $(LIBRARIES) $(DEBUG_MODE)

###
# Main object files
###

FrontendApp.o: $(SRC_DIR)/FrontendApp.cpp CommandLineParser.o SocketManager.o
	g++ -std=c++17 -o $(OBJ_DIR)/FrontendApp.o -c $(SRC_DIR)/FrontendApp.cpp $(INCLUDE_PATH) $(LIBRARIES) $(DEBUG_MODE)

CommandLineParser.o: $(SRC_DIR)/io/CommandLineParser.cpp
	g++ -std=c++17 -o $(OBJ_DIR)/CommandLineParser.o -c $(SRC_DIR)/io/CommandLineParser.cpp $(INCLUDE_PATH) $(DEBUG_MODE)

Socket.o: $(SRC_DIR)/io/Socket.cpp
	g++ -std=c++17 -o $(OBJ_DIR)/Socket.o -c $(SRC_DIR)/io/Socket.cpp $(INCLUDE_PATH) $(DEBUG_MODE)

SocketManager.o: $(SRC_DIR)/services/SocketManager.cpp Socket.o
	g++ -std=c++17 -o $(OBJ_DIR)/SocketManager.o -c $(SRC_DIR)/services/SocketManager.cpp $(INCLUDE_PATH) $(LIBRARIES) $(DEBUG_MODE)

###
# Scripts
###

build-directories:
	@[ ! -d $(OBJ_DIR) ] && mkdir $(OBJ_DIR) || true
	@[ ! -d $(BIN_DIR) ] && mkdir $(BIN_DIR) || true

clean:
	@[ -d $(OBJ_DIR) ] && rm -r $(OBJ_DIR) || true
	@[ -d $(BIN_DIR) ] && rm -r $(BIN_DIR) || true
