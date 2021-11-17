//
// Created by Laura Corssac on 11/13/21.
//

#include "../include/FrontEndPayload.hpp"

FrontEndPayload* FrontEndPayload::fromBytes(char* sessionAuthInBytes)
{
    return (FrontEndPayload*)sessionAuthInBytes;
}

char* FrontEndPayload::toBytes()
{
    return (char*)this;
}
