#include "../include/user_information.hpp"
#include "../include/NotificationManager.hpp"
#include <semaphore.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>
#include <list>

#include "../include/utils/ListExtensions.hpp"
#include "../include/GlobalManager.hpp"

//TODO: move this from here
struct arg_struct {
    userInformation::UserInformation* userInformation;
    string notificationID;
};

using namespace std;
namespace userInformation {

    UserInformation::UserInformation() { }

    UserInformation::UserInformation(string username) {
        this->username = username;
        //this->pendingNotifications = {};
        //this->followers = {};
        sem_init(&(this->freeCritialSession), 0, 1);
        sem_init(&(this->hasItems), 0, 0);
    }

    UserInformation::UserInformation(string username, list<string> pendingNotifications, list <string> followers) {
        this->username = username;
        this->pendingNotifications = pendingNotifications;
        this->followers = followers;
        sem_init(&(this->freeCritialSession), 0, 1);
        sem_init(&(this->hasItems), 0, pendingNotifications.size());
    }

    UserInformation::~UserInformation() { }

    bool UserInformation::hasSessionWithID(string sessionID) {
        return !(this->sessions.find(sessionID) == this->sessions.end());
    }

    void UserInformation::updateSession(string sessionID, SocketType type, int socketValue) {

        if (type == Common::NOTIFICATION_SOCKET) {
            this->sessions[sessionID].notif_socket = socketValue;
        } else if (type == Common::COMMAND_SOCKET) {
            this->sessions[sessionID].client_socket = socketValue;
        }

    }

    unordered_map<string, Session> UserInformation::getSessions() {
        return this->sessions;
    }

    void UserInformation::addNewSession(string sessionID, Session session) {
        this->sessions[sessionID] = session;
    }

    void UserInformation::removeSession(string sessionID) {
        this->sessions.erase(sessionID);
    }

    ErrorCodes UserInformation::addNewFollower(string follower) {
        if (find(this->followers.begin(),
            this->followers.end(), follower) == this->followers.end()) {
            this->followers.push_back(follower);
            return SUCCESS;
        }
        else {
            return ERROR;
        }
    }

    void UserInformation::setFollowers(list <string> followers) {
        this->followers = followers;
    }

    void UserInformation::setNotifications(list<string> notificationsIDs) {
        this->pendingNotifications = notificationsIDs;
    }

    list<string> UserInformation::getFollowers() {
        return this->followers;
    }

    list<string> UserInformation::getPendingNotifications() {
        return this->pendingNotifications;
    }

    string UserInformation::toString() {
        string fullString;

        fullString += "User: ";
        fullString += this->username;
        fullString += '\n';

        fullString += "Followers: ";
        for (string follower : this->followers) {
            fullString += follower + " ";
        }
        fullString += '\n';
        fullString += "Notifications: ";
        for (string notification : this->pendingNotifications) {
            fullString += notification + " ";
        }
        fullString += '\n';
        fullString += "NumberOfSessions: ";
        fullString += to_string(this->getNumberOfSessions());
        fullString += "\n\n";
        return fullString;

    }

    int UserInformation::getNumberOfSessions() {
        return this->sessions.size();
    }

    void UserInformation::stopListeningForNotifications() {
        cout << "Cancelando thread de consumo de notificações" << endl;
        if (pthread_cancel(this->consumerTid) == 0) {
            cout << "Sucesso terminando thread de consumo de notificação" << endl;
        } else {
            cout << "ERRO terminando thread de consumo de notificação" << endl;
        }
    }

    void UserInformation::produceNewNotification(string notificationID) {

        cout << "Produzindo nova notificação" << endl;

        struct arg_struct* args = new struct arg_struct;

        args->userInformation = this;
        args->notificationID.assign(notificationID);

        sem_wait(&(this->freeCritialSession));

        cout << "Produzindo notificação com ID: " << notificationID << endl;
        this->pendingNotifications.push_back(notificationID);

        sem_post(&(this->freeCritialSession));
        sem_post(&(this->hasItems));


    }

    void UserInformation::startListeningForNotifications() {

        if (pthread_create(&(this->consumerTid), NULL, this->consumer, (void*)this)) {
            cout << "ERRO criando thread de consumo" << endl;
            //free(args);
        }
    }

    void* UserInformation::consumer(void* arg) {

        cout << "Iniciando consumo de notificação" << endl;
        UserInformation* _this = (UserInformation*)arg;

        if (_this == NULL) {
            cout << "USER IS NULL" << endl;
            return 0;
        }
        cout << "USER " << _this->toString() << endl;

        while (_this->getNumberOfSessions() > 0) {
            sleep(rand() % 5);
            sem_wait(&(_this->hasItems));
            sem_wait(&(_this->freeCritialSession));

            // this if shouldn't be needed, but better safe than sorry
            if (!_this->pendingNotifications.empty()) {

                string consumedItem = _this->pendingNotifications.front();
                cout << "Consumindo notificação de ID: " << consumedItem << endl;
                _this->pendingNotifications.pop_front();
                GlobalManager::notifManager.sendNotificationTo(_this->username, consumedItem);
            }
            else {
                cout << "ERRO consumindo notificação" << endl;
            }

            sem_post(&(_this->freeCritialSession));

        }
        cout << "Finalizando thread de consumo" << endl;
        //free(_this);
        return 0;
    }

    Session UserInformation::getSessionWithID(string sessionID)
    {
        return this->sessions[sessionID];
    }
}
