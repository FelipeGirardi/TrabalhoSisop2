//
// Created by Laura Corssac on 9/14/21.
//

#include "../include/GlobalManager.hpp"

ProfileSessionManager GlobalManager::sessionManager;
NotificationManager GlobalManager::notifManager;
CommunicationManager GlobalManager::commManager;

GlobalManager::GlobalManager() {

}
//GlobalManager::GlobalManager(ProfileSessionManager profileSessionManager,
//                             NotificationManager notificationManager){
//    this->profileSessionManager = profileSessionManager;
//    this->notificationManager = notificationManager;
//}
