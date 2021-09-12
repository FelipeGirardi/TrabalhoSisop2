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
using namespace profileSessionManager;

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

    void NotificationManager::newNotificationSentBy(string username, string notification) {
        // Criar objeto notification (id, pendingReaders)
        //ProfileSessionManager::newNotificationSentBy(username, notification.getID());
    }

    int NotificationManager::getPendingReaders(string username) {
        ProfileSessionManager profSessionManager;
        UserInformation userWhoSentMessage = profSessionManager.getUserByUsername(username);
        list<string> followerNames = userWhoSentMessage.getFollowers();
        int pendingReaders = 0;
        for(string followerName : followerNames) {
            //UserInformation follower = profSessionManager.getUserByUsername(followerName);
            //int nSessions = follower.getNumberOfSessions();
            pendingReaders += 1;
        }
        return pendingReaders;
    }
}

