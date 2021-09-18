###
# Makefile to build 'app_server' application.
#
# Authors:
#   Felipe Girardi 
#   Laura Corssac
#   Marlize Ramos
#   Renan Kummer
###

BUILD_DIR = build
SRC_DIR = src
OBJ_DIR = out
BIN_DIR = bin
COMMON_OBJ_DIR = ../common/$(OBJ_DIR)

#all: build-directories FileManager.o NotificationManager.o CommunicationManager.o profile_session_manager.o user_information.o tcp_server.o utils/StringExtensions.o
#	g++ -o $(BIN_DIR)/serverApp $(OBJ_DIR)/*.o $(COMMON_OBJ_DIR)/*.o -lpthread

build_server:
	g++ -std=c++11 -c src/tcp_server.cpp src/profile_session_manager.cpp \
	src/user_information.cpp \
	src/utils/StringExtensions.cpp \
	src/FileManager.cpp \
	../common/src/Notification.cpp \
	src/NotificationManager.cpp \
	src/utils/ListExtensions.cpp \
	src/GlobalManager.cpp \
	src/CommunicationManager.cpp \
	../common/src/Packet.cpp \
	../common/src/PacketType.cpp
	mv *.o $(BUILD_DIR)
	g++ -o $(BUILD_DIR)/serverApp build/*.o -lpthread

###
# Object files
###
#
#FileManager.o: $(SRC_DIR)/FileManager.cpp
#	g++ -std=c++11 -o $(OBJ_DIR)/FileManager.o -c $(SRC_DIR)/FileManager.cpp
#
#NotificationManager.o: $(SRC_DIR)/NotificationManager.cpp
#	g++ -std=c++11 -o $(OBJ_DIR)/NotificationManager.o -c $(SRC_DIR)/NotificationManager.cpp
#
#profile_session_manager.o: $(SRC_DIR)/profile_session_manager.cpp
#	g++ -std=c++11 -o $(OBJ_DIR)/profile_session_manager.o -c $(SRC_DIR)/profile_session_manager.cpp
#
#CommunicationManager.o:
#	g++ -std=c++11 -o $(OBJ_DIR)/CommunicationManager.o -c $(SRC_DIR)/CommunicationManager.cpp
#
#user_information.o:
#	g++ -std=c++11 -o $(OBJ_DIR)/user_information.o -c $(SRC_DIR)/user_information.cpp
#
#tcp_server.o: $(SRC_DIR)/tcp_server.cpp
#	g++ -std=c++11 -o $(OBJ_DIR)/tcp_server.o -c $(SRC_DIR)/tcp_server.cpp
#
#utils/StringExtensions.o:
#	g++ -std=c++11 -o $(OBJ_DIR)/StringExtensions.o -c $(SRC_DIR)/utils/StringExtensions.cpp

###
# Scripts
###

#main: common all
#	@./$(BIN_DIR)/serverApp
#
#common:
#	@$(MAKE) -C ../common
#
#build-directories:
#	@[ ! -d $(OBJ_DIR) ] && mkdir $(OBJ_DIR) || true
#	@[ ! -d $(BIN_DIR) ] && mkdir $(BIN_DIR) || true
#
#clean:
#	@[ -d $(OBJ_DIR) ] && rm -r $(OBJ_DIR) || true
#	@[ -d $(BIN_DIR) ] && rm -r $(BIN_DIR) || true
#
#clean:
#	rm $(BUILD_DIR)/*

laura:
	make build_server
	./$(BUILD_DIR)/serverApp
