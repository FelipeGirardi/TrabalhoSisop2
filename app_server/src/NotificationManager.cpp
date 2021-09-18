//
// Created by Laura Corssac on 9/9/21.
//

#include "../include/NotificationManager.hpp"
#include "../../Common/include/Notification.hpp"
#include <time.h>
#include "../include/profile_session_manager.hpp"
#include "../include/GlobalManager.hpp"
#include "../include/CommunicationManager.hpp"
#include <algorithm>
#include <unordered_map>
#include <string>
#include <list>
#include <semaphore.h>

using namespace std;
using namespace notification;
using namespace profileSessionManager;

    NotificationManager::NotificationManager() { }

    void NotificationManager::setNotifications(unordered_map <string, Notification> notifications) {
        this->notifications = notifications;

        list<int> keys;
        for(auto kv : notifications) { keys.push_back(stoi(kv.first)); }
        this->idNextNotification = *(max_element(keys.begin(), keys.end())) + 1;
        sem_init(&(this->freeCritialSession), 0, 1);

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

    void NotificationManager::addNewNotification(Notification notification) {

    }

    void NotificationManager::sendNotificationTo(string username, string notificationID) {

        cout << "sendNotificationTo " << username << " " << notificationID << endl;

        if (this->notifications.find(notificationID) == this->notifications.end()) {
            cout << "not possible to send notification bc it doesn't exist" << endl;
            return;
        }
        unordered_map<string, UserInformation> users = GlobalManager::sessionManager.getUsers();
        if (users.find(username) == users.end()) {
            cout << "not possible to send notification bc user doesn't exist" << endl;
            return;
        }

        list<Session> sessions = users[username].sessions;
        int quantitySent = GlobalManager::commManager.sendNotificationToSessions(sessions,
                                                              this->notifications[notificationID]);

        cout << quantitySent << " notifications sent" << endl;
    }

    void NotificationManager::newNotificationSentBy(string username, string notification) {

        long int currentTime = static_cast<long int> (time(NULL));
        int pendingReaders = this->getPendingReaders(username);

        sem_wait(&freeCritialSession);
        string notificationID = to_string(this->idNextNotification);
        Notification newNotification = Notification(notificationID,
                                                    notification, username,
                                                    currentTime,
                                                    pendingReaders);
        this->notifications[notificationID] = newNotification;
        idNextNotification ++;
        sem_post(&freeCritialSession);

        GlobalManager::sessionManager.newNotificationSentBy(username,
                                                            notificationID);
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

