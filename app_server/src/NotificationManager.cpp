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
        if (GlobalManager::notifManager.notifications.find(notificationID) == this->notifications.end()) {
            return Notification();
        }
        else {
            return GlobalManager::notifManager.notifications[notificationID];
        }
    }

    void NotificationManager::addNewNotification(Notification notification) {

    }

    void NotificationManager::sendNotificationTo(string username, string notificationID) {

        cout << "Iniciando envio de notificação id " << notificationID << " às sessões de " << username << endl;

        if (GlobalManager::notifManager.notifications.find(notificationID) == GlobalManager::notifManager.notifications.end()) {
            cout << "ERRO mandando notificação. Notificação não existe." << endl;
            return;
        }
        unordered_map<string, UserInformation> users = GlobalManager::sessionManager.getUsers();
        if (users.find(username) == users.end()) {
            cout << "ERRO mandando notificação. Usuário não existe." << endl;
            return;
        }
        list<Session> sessions;
        for (auto kv : users[username].getSessions()) {
            sessions.push_back(kv.second);
        }
        ErrorCodes sent = GlobalManager::commManager.sendNotificationToSessions(sessions,
                                                                          this->notifications[notificationID]);

        if (sent == SUCCESS) {
            GlobalManager::notifManager.notifications[notificationID].decrementPendingReaders();
        }
        int pendingReaders = GlobalManager::notifManager.notifications[notificationID].getPendingReaders();

        if (pendingReaders == 0) {
            cout << "Notificação foi mandada para todos os usuários pendentes. deletando." << endl;
            GlobalManager::notifManager.notifications.erase(notificationID);
        } else {
            cout << "Notificação pendente a " <<  pendingReaders << " usuários" << endl;
        }

    }

    void NotificationManager::newNotificationSentBy(string username, string notification) {

        long int currentTime = static_cast<long int> (time(NULL));
        int pendingReaders = GlobalManager::notifManager.getPendingReaders(username);
        if (pendingReaders == 0) {
            cout << "Usuário não tem nenhum seguidor. Notificação ignorada." << endl;
            return;
        }

        sem_wait(&(GlobalManager::notifManager.freeCritialSession));
        string notificationID = to_string(GlobalManager::notifManager.idNextNotification);
        Notification newNotification = Notification(notificationID,
                                                    notification, username,
                                                    currentTime,
                                                    pendingReaders);
        GlobalManager::notifManager.notifications[notificationID] = newNotification;
        idNextNotification ++;
        sem_post(&(GlobalManager::notifManager.freeCritialSession));

        GlobalManager::sessionManager.newNotificationSentBy(username,
                                                            notificationID);
    }

    int NotificationManager::getPendingReaders(string username) {
        UserInformation userWhoSentMessage = GlobalManager::sessionManager.getUserByUsername(username);
        return userWhoSentMessage.getFollowers().size();
    }

