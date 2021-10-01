//
// Created by Laura Corssac on 10/1/21.
//

#include "../include/Session.hpp"

#define UNDEFINED_SOCKET -1

Session::Session() {
    this->notif_socket = UNDEFINED_SOCKET;
    this->client_socket = UNDEFINED_SOCKET;
}
bool Session::hasNotifSocket() {
    return this->notif_socket == UNDEFINED_SOCKET;
}

bool Session::hasCommandSocket() {
    return this->client_socket == UNDEFINED_SOCKET;
}

