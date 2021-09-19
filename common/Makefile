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

all: build-directories Notification.o Packet.o PacketType.o

###
# Object files
###

Notification.o: $(SRC_DIR)/Notification.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/Notification.o -c $(SRC_DIR)/Notification.cpp

Packet.o: $(SRC_DIR)/Packet.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/Packet.o -c $(SRC_DIR)/Packet.cpp

PacketType.o: $(SRC_DIR)/PacketType.cpp
	g++ -std=c++11 -o $(OBJ_DIR)/PacketType.o -c $(SRC_DIR)/PacketType.cpp

###
# Scripts
###

build-directories:
	@[ ! -d $(OBJ_DIR) ] && mkdir $(OBJ_DIR) || true

clean:
	@[ -d $(OBJ_DIR) ] && rm -r $(OBJ_DIR) || true
