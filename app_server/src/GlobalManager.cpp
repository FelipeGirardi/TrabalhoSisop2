//
// Created by Laura Corssac on 9/14/21.
//

#include "../include/GlobalManager.hpp"

ProfileSessionManager GlobalManager::sessionManager;
NotificationManager GlobalManager::notifManager;
CommunicationManager GlobalManager::commManager;
ElectionManager GlobalManager::electionManager;

using namespace notification;

GlobalManager::GlobalManager() { }

void GlobalManager::printItself() {

    cout << endl << endl;
    cout << " ---- REGISTERED USERS ---- " << endl << endl;

    for (auto user : GlobalManager::sessionManager.getUsers()) {
        string key = user.first;
        UserInformation userInfo = user.second;
        cout << userInfo.toString() << endl;
    }

    cout << endl << endl;
    cout << " ---- REGISTERED NOTIFICATIONS ---- " << endl << endl;

    for (auto notification : GlobalManager::notifManager.getNotifications()) {
        Notification notif = notification.second;
        cout << notif.toString() << endl;
    }
}