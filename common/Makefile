###
# Makefile to build common functions and data structures for all projects.
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
HEADERS_DIR = include
INCLUDE_PATH = -I$(HEADERS_DIR)
DEBUG_MODE = -g

all: build-directories Notification.o Packet.o PacketType.o StringExtensions.o SessionAuth.o Socket.o

###
# Main object files
###

Notification.o: $(SRC_DIR)/Notification.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/Notification.o -c $(SRC_DIR)/Notification.cpp $(INCLUDE_PATH)

Packet.o: $(SRC_DIR)/Packet.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/Packet.o -c $(SRC_DIR)/Packet.cpp $(INCLUDE_PATH)

PacketType.o: $(SRC_DIR)/PacketType.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/PacketType.o -c $(SRC_DIR)/PacketType.cpp $(INCLUDE_PATH)

SessionAuth.o: $(SRC_DIR)/SessionAuth.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/SessionAuth.o -c $(SRC_DIR)/SessionAuth.cpp $(INCLUDE_PATH) -luuid

Socket.o: $(SRC_DIR)/Socket.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/Socket.o -c $(SRC_DIR)/Socket.cpp $(INCLUDE_PATH)

StringExtensions.o: $(SRC_DIR)/StringExtensions.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/StringExtensions.o -c $(SRC_DIR)/StringExtensions.cpp $(INCLUDE_PATH)

###
# Test object files
###

NotificationTest.o: $(TEST_DIR)/NotificationTest.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/NotificationTest.o -c $(TEST_DIR)/NotificationTest.cpp $(INCLUDE_PATH)

SessionAuthTest.o: $(TEST_DIR)/SessionAuthTest.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/SessionAuthTest.o -c $(TEST_DIR)/SessionAuthTest.cpp $(INCLUDE_PATH)

###
# Test executables
###

NotificationTest: NotificationTest.o Notification.o StringExtensions.o
	g++ -std=c++11 -o $(BIN_DIR)/NotificationTest.out $(OBJ_DIR)/NotificationTest.o $(OBJ_DIR)/Notification.o $(OBJ_DIR)/StringExtensions.o

SessionAuthTest: SessionAuthTest.o SessionAuth.o
	g++ -std=c++11 -o $(BIN_DIR)/SessionAuthTest.out $(OBJ_DIR)/SessionAuthTest.o $(OBJ_DIR)/SessionAuth.o -luuid

###
# Scripts
###

build-directories:
	@[ ! -d $(OBJ_DIR) ] && mkdir $(OBJ_DIR) || true
	@[ ! -d $(BIN_DIR) ] && mkdir $(BIN_DIR) || true

check: build-directories NotificationTest SessionAuthTest
	./$(BIN_DIR)/NotificationTest.out; ./$(BIN_DIR)/SessionAuthTest.out

clean:
	@[ -d $(OBJ_DIR) ] && rm -r $(OBJ_DIR) || true
	@[ -d $(BIN_DIR) ] && rm -r $(BIN_DIR) || true
