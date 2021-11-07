//
// Created by Laura Corssac on 9/17/21.
//

#include "../include/Packet.hpp"
#include "../include/PacketType.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

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
    cout << "type: " << stringDescribingType(this->type) << endl;
    printf("length: %d\n", this->length);
    printf("timestamp: %ld\n", this->timestamp);
    printf("payload: %s\n", this->_payload);
}
