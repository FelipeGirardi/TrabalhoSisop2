//
// Created by Laura Corssac on 9/9/21.
//

#include "../include/NotificationManager.hpp"
#include "../include/utils/ErrorCodes.hpp"
#include "../../Common/include/Notification.hpp"
#include <time.h>
#include "../include/ProfileSessionManager.hpp"
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

    NotificationManager::NotificationManager() {
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
//        list<Session> sessions;
//        for (auto kv : users[username].getSessions()) {
//            sessions.push_back(kv.second);
//        }

        ErrorCodes sent;
        cout << "Mandando notificação para RMS " << endl;
        sent = GlobalManager::commManager.sendNotificationToRMs(username,
                                                                                 this->notifications[notificationID].getID());

        if (sent) {
            cout << "Pedido de deleção de notificação mandado para RMS" << endl;
            cout << "Mandando notificação para front ends" << endl;
            sent = GlobalManager::commManager.sendNotificationToFrontEnds(username,
                                                                     this->notifications[notificationID]);
        }

        if (sent == SUCCESS) {
            cout << "Notificação mandada para front ends" << endl;
            GlobalManager::notifManager.notifications[notificationID].decrementPendingReaders();
        }

        int pendingReaders = GlobalManager::notifManager.notifications[notificationID].getPendingReaders();

        if (pendingReaders == 0) {
            cout << "Notificação foi mandada para todos os usuários pendentes. Deletando." << endl;
            GlobalManager::notifManager.notifications.erase(notificationID);
        } else {
            cout << "Notificação ainda é pendente a " <<  pendingReaders << " usuários" << endl;
        }

    }



    void NotificationManager::newNotificationSentBy(string username, string notification) {

        long int currentTime = static_cast<long int> (time(NULL));
        int pendingReaders = GlobalManager::notifManager.getPendingReaders(username);
        cout << "Nova notificação é pendente a " << pendingReaders << " usuários" << endl;
        if (pendingReaders == 0) {
            cout << "Usuário não tem nenhum seguidor. Notificação ignorada." << endl;
            return;
        }

        sem_wait(&(GlobalManager::notifManager.freeCritialSession));
        string notificationID = to_string(GlobalManager::notifManager.idNextNotification);
        cout << "ID nova notificação = " << notificationID << endl;
        Notification newNotification = Notification(notificationID,
                                                    notification, username,
                                                    currentTime,
                                                    pendingReaders);
        GlobalManager::notifManager.notifications[notificationID] = newNotification;
        GlobalManager::notifManager.idNextNotification ++;
        sem_post(&(GlobalManager::notifManager.freeCritialSession));

        GlobalManager::sessionManager.newNotificationSentBy(username,
                                                            notificationID);
    }

    int NotificationManager::getPendingReaders(string username) {
        UserInformation userWhoSentMessage = GlobalManager::sessionManager.getUserByUsername(username);
        return userWhoSentMessage.getFollowers().size();
    }

