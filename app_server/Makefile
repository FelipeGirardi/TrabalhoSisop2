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
COMMON_DIR = ../common
COMMON_OBJ_DIR = $(COMMON_DIR)/$(OBJ_DIR)

all: build-directories common CommunicationManager.o FileManager.o GlobalManager.o \
         NotificationManager.o profile_session_manager.o tcp_server.o \
		 user_information.o utils/StringExtensions.o Session.o

	g++ -o $(BIN_DIR)/serverApp $(COMMON_OBJ_DIR)/*.o $(OBJ_DIR)/*.o -luuid -lpthread
###
# Object files
###

CommunicationManager.o: $(SRC_DIR)/CommunicationManager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/CommunicationManager.o -c $(SRC_DIR)/CommunicationManager.cpp

FileManager.o: $(SRC_DIR)/FileManager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/FileManager.o -c $(SRC_DIR)/FileManager.cpp

GlobalManager.o: $(SRC_DIR)/GlobalManager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/GlobalManager.o -c $(SRC_DIR)/GlobalManager.cpp

NotificationManager.o: $(SRC_DIR)/NotificationManager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/NotificationManager.o -c $(SRC_DIR)/NotificationManager.cpp

profile_session_manager.o: $(SRC_DIR)/profile_session_manager.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/profile_session_manager.o -c $(SRC_DIR)/profile_session_manager.cpp

tcp_server.o: $(SRC_DIR)/tcp_server.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/tcp_server.o -c $(SRC_DIR)/tcp_server.cpp

user_information.o: $(SRC_DIR)/user_information.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/user_information.o -c $(SRC_DIR)/user_information.cpp

utils/StringExtensions.o:
	g++ -std=c++11 -o $(OBJ_DIR)/StringExtensions.o -c $(SRC_DIR)/utils/StringExtensions.cpp

Session.o:
	g++ -std=c++11 -o $(OBJ_DIR)/Session.o -c $(SRC_DIR)/Session.cpp

###
# Scripts
###

main: all
	@./$(BIN_DIR)/serverApp

common:
	@[ ! -f $(COMMON_OBJ_DIR)/Notification.o -a ! -f $(COMMON_OBJ_DIR)/Packet.o \
	    -a ! -f $(COMMON_OBJ_DIR)/PacketType.o \
	    -a ! -f $(COMMON_OBJ_DIR)/SessionAuth.o ] && $(MAKE) -C $(COMMON_DIR) || true

build-directories:
	@[ ! -d $(OBJ_DIR) ] && mkdir $(OBJ_DIR) || true
	@[ ! -d $(BIN_DIR) ] && mkdir $(BIN_DIR) || true

clean:
	@[ -d $(OBJ_DIR) ] && rm -r $(OBJ_DIR) || true
	@[ -d $(BIN_DIR) ] && rm -r $(BIN_DIR) || true


