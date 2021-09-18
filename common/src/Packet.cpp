//
// Created by Laura Corssac on 9/17/21.
//

#include "../include/Packet.hpp"
#include <stdio.h>

void Packet::printItself() {
    printf("type: %d\n", this->type);
    printf("length: %d\n", this->length);
    printf("timestamp: %ld\n", this->timestamp);
    printf("payload: %s\n", this->_payload);
}
