//
// Created by Laura Corssac on 9/14/21.
//

#ifndef GLOBALMANAGER_HPP
#define GLOBALMANAGER_HPP

#include "profile_session_manager.hpp"
#include "NotificationManager.hpp"

using namespace profileSessionManager;

class GlobalManager {
public:
    static ProfileSessionManager sessionManager;
    static NotificationManager notifManager;
    GlobalManager();


};


#endif //GLOBALMANAGER_HPP
