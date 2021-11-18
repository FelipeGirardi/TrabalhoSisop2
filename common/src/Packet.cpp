//
// Created by Laura Corssac on 9/17/21.
//

#include "../include/Packet.hpp"
#include <cstdio>
#include <cstring>

Packet* Packet::fromBytes(const char* bytes)
{
    if (bytes != nullptr)
    {
        auto parsedNotification = (Packet*)bytes;
        return new Packet(*parsedNotification);
    }

    return nullptr;
}

char* Packet::toBytes()
{
    auto packetCopy = new Packet(*this);
    return (char*)packetCopy;
}

void Packet::printItself() {
    printf("type: %d\n", this->type);
    printf("length: %d\n", this->length);
    printf("timestamp: %ld\n", this->timestamp);
    printf("payload: %s\n", this->_payload);
}
