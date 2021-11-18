//
// Created by Laura Corssac on 9/17/21.
//

#include "../include/PacketType.hpp"

std::string stringDescribingType(PacketType type) {
    switch (type) {
        case SEND:
            return "SEND";
        case FOLLOW:
            return "FOLLOW";
        case EXIT:
            return "EXIT";
        case LOGIN:
            return "LOGIN";
        case USERNAME:
            return "USERNAME";
        case KEEP_ALIVE:
            return "KEEP_ALIVE";
        case ELECTION:
            return "ELECTION";
        case COORDINATOR:
            return "COORDINATOR";
        case ANSWER:
            return "ANSWER";
        case HELLO_SEND:
            return "HELLO_SEND";
        case HELLO_RECEIVE:
            return "HELLO_RECEIVE";
        case EXIT_SERVER:
            return "EXIT_SERVER";
        default:
            return "";
    }
}