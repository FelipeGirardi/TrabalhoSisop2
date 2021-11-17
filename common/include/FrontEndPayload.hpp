//
// Created by Laura Corssac on 11/13/21.
//

#ifndef FRONTENDPAYLOAD_HPP
#define FRONTENDPAYLOAD_HPP

#include <string>

using namespace std;

class FrontEndPayload {

public:
    char commandContent[100];
    char senderUsername[100];
    static FrontEndPayload* fromBytes(char* sessionAuthInBytes);
    char* toBytes();

};

#endif //FRONTENDPAYLOAD_HPP
