//
// Created by Laura Corssac on 9/14/21.
//

#ifndef GLOBALMANAGER_HPP
#define GLOBALMANAGER_HPP

#include "ProfileSessionManager.hpp"
#include "NotificationManager.hpp"
#include "CommunicationManager.hpp"
#include "ElectionManager.hpp"
#include "FrontEndCommunicationManager.hpp"

using namespace profileSessionManager;
using namespace communicationManager;

class GlobalManager {
public:
    static ProfileSessionManager sessionManager;
    static NotificationManager notifManager;
    static CommunicationManager commManager;
    static ElectionManager electionManager;
    static FrontEndCommunicationManager frontEndManager;
    static void printItself();
    GlobalManager();

};


#endif //GLOBALMANAGER_HPP
