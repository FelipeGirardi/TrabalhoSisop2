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
        case USERNAME:
            return "USERNAME";
        default:
            return "";
    }
}