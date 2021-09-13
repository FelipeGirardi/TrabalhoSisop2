###
# Makefile to build 'app_server' application.
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
COMMON_OBJ_DIR = ../common/$(OBJ_DIR)

all: build-directories MainTest.o FileManager.o NotificationManager.o profile_session_manager.o user_information.o utils/StringExtensions.o
	g++ -o $(BIN_DIR)/serverApp $(OBJ_DIR)/*.o $(COMMON_OBJ_DIR)/*.o -lpthread

###
# Object files
###

MainTest.o: MainTest.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/MainTest.o -c MainTest.cpp

FileManager.o: $(SRC_DIR)/FileManager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/FileManager.o -c $(SRC_DIR)/FileManager.cpp

NotificationManager.o: $(SRC_DIR)/NotificationManager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/NotificationManager.o -c $(SRC_DIR)/NotificationManager.cpp

profile_session_manager.o: $(SRC_DIR)/profile_session_manager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/profile_session_manager.o -c $(SRC_DIR)/profile_session_manager.cpp

tcp_server.o: $(SRC_DIR)/tcp_server.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/tcp_server.o -c $(SRC_DIR)/tcp_server.cpp

user_information.o:
	g++ -std=c++11 -o $(OBJ_DIR)/user_information.o -c $(SRC_DIR)/user_information.cpp

utils/StringExtensions.o:
	g++ -std=c++11 -o $(OBJ_DIR)/StringExtensions.o -c $(SRC_DIR)/utils/StringExtensions.cpp

###
# Scripts
###

main: common all
	@./$(BIN_DIR)/serverApp

common:
	@$(MAKE) -C ../common

build-directories:
	@[ ! -d $(OBJ_DIR) ] && mkdir $(OBJ_DIR) || true
	@[ ! -d $(BIN_DIR) ] && mkdir $(BIN_DIR) || true

clean:
	@[ -d $(OBJ_DIR) ] && rm -r $(OBJ_DIR) || true
	@[ -d $(BIN_DIR) ] && rm -r $(BIN_DIR) || true
