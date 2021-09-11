//
// Created by Laura Corssac on 9/9/21.
//

#include "../include/NotificationManager.hpp"
#include "../include/profile_session_manager.hpp"
#include "../../Common/include/Notification.hpp"
#include <unordered_map>
#include <string>

using namespace std;
using namespace notification;

namespace notifManager {

    NotificationManager::NotificationManager() {}

    NotificationManager::NotificationManager(unordered_map <string, Notification> notifications) {
        this->setNotifications(notifications);
    }

    void NotificationManager::setNotifications(unordered_map <string, Notification> notifications) {
        this->notifications = notifications;
    }

    unordered_map <string, Notification> NotificationManager::getNotifications() {
        return this->notifications;
    }

    Notification NotificationManager::getNotificationByID(string notificationID) {
        if (this->notifications.find(notificationID) == this->notifications.end()) {
            return Notification();
        }
        else {
            return this->notifications[notificationID];
        }
    }

    void NotificationManager::sendNotificationTo(string username, string notificationID) {
        if (this->notifications.find(notificationID) == this->notifications.end()) {}
        else {
            //CommunicationManager::sendNotificationTo(username, this->notifications[notificationID]);
        }
    }

    void NotificationManager::newNotificationSentBy(string username, Notification notification) {
        //ProfileSessionManager::newNotificationSentBy(username, notification.getID());
    }
}

