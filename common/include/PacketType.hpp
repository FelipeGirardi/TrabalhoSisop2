//
// Created by Laura Corssac on 9/17/21.
//

#ifndef PACKETTYPE_HPP
#define PACKETTYPE_HPP

#include <string>

typedef enum PacketType {

    NOTIFICATION = 1, // server -> client
    USERNAME, // client -> server
    SEND, // client -> server
    FOLLOW, // client -> server
    SERVER_ACK, // server -> client
    SERVER_ERROR, // server -> client
    LOGIN,
    EXIT, // client -> server or server -> client

    KEEP_ALIVE, // secondary server -> primary server
    ELECTION,
    ANSWER,
    COORDINATOR,

    HELLO_SEND, // server -> server
    HELLO_RECEIVE,

    EXIT_SERVER

}PacketType;

std::string stringDescribingType(PacketType type);

#endif //PACKETTYPE_HPP
