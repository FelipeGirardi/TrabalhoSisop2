//
// Created by Laura Corssac on 9/14/21.
//

#include "../include/GlobalManager.hpp"

ProfileSessionManager GlobalManager::sessionManager;
NotificationManager GlobalManager::notifManager;
CommunicationManager GlobalManager::commManager;

using namespace notification;

GlobalManager::GlobalManager() {

}

void GlobalManager::printItself() {
    for (auto user : GlobalManager::sessionManager.getUsers()) {
        string key = user.first;
        UserInformation userInfo = user.second;
        cout << "key = " << key << endl;
        cout << userInfo.toString() << endl;
    }

    cout << "notifications" << endl << endl;

    for (auto notification : GlobalManager::notifManager.getNotifications()) {
        Notification notif = notification.second;
        cout << notif.toString() << endl;
    }
}

//GlobalManager::GlobalManager(ProfileSessionManager profileSessionManager,
//                             NotificationManager notificationManager){
//    this->profileSessionManager = profileSessionManager;
//    this->notificationManager = notificationManager;
//}
