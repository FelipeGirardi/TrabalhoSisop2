//
// Created by Laura Corssac on 11/13/21.
//

#ifndef FRONTENDCOMMUNICATIONMANAGER_HPP
#define FRONTENDCOMMUNICATIONMANAGER_HPP

#include "ServerAndFrontEndInfo.h"
#include "utils/ErrorCodes.hpp"

#include <vector>

using namespace std;

class FrontEndCommunicationManager {

private:
    vector<FrontEndInfo> frontEnds;
    ErrorCodes sendHelloToFrontEnd(FrontEndInfo frontEndInfo);
    static void *connectToFrontEnd(void *data);

public:
    void setFrontEnds(vector<FrontEndInfo> frontEnds);
    vector<FrontEndInfo> getFrontEnds();
    void sendHelloToFrontEnds();
    ErrorCodes sendHelloToFrontEnd(FrontEndInfo frontEndInfo, PacketType type, int idFrontEnd);
    void setSendSocket(int socket, int _id);
    void setReceiveSocket(int socket, int _id);
    static void *client_thread_func(void *data);

};

#endif //FRONTENDCOMMUNICATIONMANAGER_HPP
