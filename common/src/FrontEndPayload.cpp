//
// Created by Laura Corssac on 11/13/21.
//

#include "../include/FrontEndPayload.hpp"

FrontEndPayload* FrontEndPayload::fromBytes(char* sessionAuthInBytes)
{
    FrontEndPayload *parsedFrontEndPayload = (FrontEndPayload*)sessionAuthInBytes;

//    FrontEndPayload *sessionAuthCopy;
//    sessionAuthCopy->commandContent = parsedFrontEndPayload->commandContent;
//    sessionAuthCopy->senderUsername = parsedFrontEndPayload->senderUsername;
    return parsedFrontEndPayload;
}

char* FrontEndPayload::toBytes()
{
    FrontEndPayload *sessionAuthCopy = new FrontEndPayload;
//    sessionAuthCopy->commandContent = this->commandContent;
//    sessionAuthCopy->senderUsername = this->senderUsername;
    return (char*)sessionAuthCopy;
}
