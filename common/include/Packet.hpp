//
// Created by Laura Corssac on 9/17/21.
//

#ifndef PACKET_HPP
#define PACKET_HPP

#include "../include/PacketType.hpp"

#define BUFFER_SIZE 256

class Packet
{
public:
    PacketType type;            // Tipo do pacote
    int length;                 // Comprimento do payload
    long int timestamp;         // Timestamp
    char _payload[BUFFER_SIZE]; // Dados do pacote

    static Packet* fromBytes(const char* bytes);

    char* toBytes();
    void printItself();
};

#endif //PACKET_HPP
