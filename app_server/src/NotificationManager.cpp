//
// Created by Laura Corssac on 9/9/21.
//

#include "../include/NotificationManager.hpp"
#include "../include/utils/ErrorCodes.hpp"
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

        cout << "iniciando envio de notificação id " << notificationID << " às sessões de " << username << endl;

        if (this->notifications.find(notificationID) == this->notifications.end()) {
            cout << "erro mandando notificação. notificação não existe." << endl;
            return;
        }
        unordered_map<string, UserInformation> users = GlobalManager::sessionManager.getUsers();
        if (users.find(username) == users.end()) {
            cout << "erro mandando notificação. usuário não existe." << endl;
            return;
        }
        list<Session> sessions;
        for (auto kv : users[username].getSessions()) {
            cout << "SESSION " << kv.first << endl;
            cout << "notif socket " << kv.second.notif_socket << endl;
            cout << "command socket " << kv.second.notif_socket << endl;
            sessions.push_back(kv.second);
        }
        ErrorCodes sent = GlobalManager::commManager.sendNotificationToSessions(sessions,
                                                                          this->notifications[notificationID]);

        if (sent == SUCCESS) {
            this->notifications[notificationID].decrementPendingReaders();
        }
        int pendingReaders = this->notifications[notificationID].getPendingReaders();

        if (pendingReaders == 0) {
            cout << "notificação foi mandada para todos os usuários pendentes. deletando." << endl;
            this->notifications.erase(notificationID);
        } else {
            cout << "notificação pendente a " <<  pendingReaders << " usuários" << endl;
        }

    }

    void NotificationManager::newNotificationSentBy(string username, string notification) {

        long int currentTime = static_cast<long int> (time(NULL));
        int pendingReaders = this->getPendingReaders(username);
        if (pendingReaders == 0) {
            cout << "Usuário não tem nenhum seguidor. Notificação ignorada." << endl;
            return;
        }

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
        UserInformation userWhoSentMessage = GlobalManager::sessionManager.getUserByUsername(username);
        return userWhoSentMessage.getFollowers().size();
    }

