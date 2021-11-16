//
// Created by Laura Corssac on 11/13/21.
//

#ifndef SERVERANDFRONTENDINFO_H
#define SERVERANDFRONTENDINFO_H

#include "../../common/include/PacketType.hpp"
#include <string>

using namespace std;

typedef struct {

    string ip;
    int port;
    int sendSocket;
    int receiveSocket;

} FrontEndInfo;

typedef struct {

    int _id;
    int sendSocket;
    int receiveSocket;
    string ip;

} ServerInfo;

typedef struct {
    string ip;
    int port;
    PacketType typeOfPacket; //SEND ou RECEIVE
} ServerArguments;

#endif //SERVERANDFRONTENDINFO_H
