//
// Created by Laura Corssac on 9/14/21.
//

#ifndef GLOBALMANAGER_HPP
#define GLOBALMANAGER_HPP

#include "profile_session_manager.hpp"
#include "NotificationManager.hpp"
#include "CommunicationManager.hpp"

using namespace profileSessionManager;
using namespace communicationManager;

class GlobalManager {
public:
    static ProfileSessionManager sessionManager;
    static NotificationManager notifManager;
    static CommunicationManager commManager;
    static void printItself();
    GlobalManager();

};


#endif //GLOBALMANAGER_HPP
