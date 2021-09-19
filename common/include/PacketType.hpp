//
// Created by Laura Corssac on 9/17/21.
//

#ifndef PACKETTYPE_HPP
#define PACKETTYPE_HPP


typedef enum PacketType {

    NOTIFICATION = 0, // server -> client
    USERNAME, // client -> server
    SEND, // client -> server
    FOLLOW, // client -> server
    SERVER_ACK, // server -> client
    SERVER_ERROR, // server -> client
    EXIT // client -> server

}PacketType;


#endif //PACKETTYPE_HPP
