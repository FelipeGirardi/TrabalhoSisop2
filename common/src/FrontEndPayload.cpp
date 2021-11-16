//
// Created by Laura Corssac on 11/13/21.
//

#include "../include/FrontEndPayload.hpp"

FrontEndPayload* FrontEndPayload::fromBytes(char* sessionAuthInBytes)
{
    FrontEndPayload *parsedFrontEndPayload = (FrontEndPayload*)sessionAuthInBytes;

//    auto sessionAuthCopy = new FrontEndPayload(
//            parsedFrontEndPayload->commandContent,
//            parsedSessionAuth->getSocketType(),
//            (char*)parsedSessionAuth->getUuid().c_str()
//    );

    return parsedFrontEndPayload;
}

char* FrontEndPayload::toBytes()
{
    //auto sessionAuthCopy = new SessionAuth(_profileId, _socketType, _uuid);
    return (char*)this;
}
