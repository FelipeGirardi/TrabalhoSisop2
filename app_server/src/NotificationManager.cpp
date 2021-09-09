//
// Created by Laura Corssac on 9/9/21.
//

#include "../include/NotificationManager.hpp"
#include "../../Common/include/Notification.hpp"
#include <unordered_map>
#include <string>

using namespace notification;
using namespace std;

NotificationManager::NotificationManager() {

}

void NotificationManager::setNotifications(unordered_map<string, Notification> notifications) {
    this->notifications = notifications;
}

unordered_map<string,Notification> NotificationManager::getNotifications() {
    return this->notifications;
}

Notification NotificationManager::getNotificationByID(string notificationID) {
    return this->notifications[notificationID];
}

void NotificationManager::sendNotificationTo(string username, string notificationID) {

}

void NotificationManager::newNotificationSentBy(string username, Notification notification) {

}

