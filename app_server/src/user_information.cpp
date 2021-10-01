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
    userInformation::UserInformation *userInformation;
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
        return !(this->sessions.find(username) == this->sessions.end());
    }

    void UserInformation::addNewSession(string sessionID, Session session) {
        this->sessions[sessionID] = session;
    }

    void UserInformation::removeSession(string sessionID) {
        this->sessions.erase(sessionID);
    }

    int UserInformation::addNewFollower(string follower) {
        if (find(this->followers.begin(),
                 this->followers.end(), follower) == this->followers.end()) {
            this->followers.push_back(follower);
            return 1;
        } else {
            return -1;
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
        pthread_cancel(this->consumerTid);
    }

    void UserInformation::produceNewNotification(string notificationID) {

        cout << "Produzindo nova notificação" << endl;

        struct arg_struct *args = new struct arg_struct;

        args->userInformation = this;
        args->notificationID.assign(notificationID);

//        pthread_t tid = (pthread_t *) malloc(sizeof (pthread_t));
//        *tid = (unsigned long int) rand();
        pthread_t tid;

        if (pthread_create(&tid, NULL, this->producer, (void *) args)) {
            cout << "ERRO criando thread de produção" << endl;
            //free(args);
        }

        // será que é assim?
        //pthread_join(*tid, NULL);
        //free(tid);

    }

    void UserInformation::startListeningForNotifications() {

        if (pthread_create(&(this->consumerTid), NULL, this->consumer, (void *) this)) {
            cout << "ERRO criando thread de consumo" << endl;
            //free(args);
        }
    }

    void * UserInformation::producer(void *arg) {

        cout << "Iniciando thread de produção de notificação" << endl;

        struct arg_struct *my_arg_struct = (struct arg_struct *) arg;
        UserInformation *_this = my_arg_struct->userInformation;
        sleep(rand()%5);

        sem_wait(&((*_this).freeCritialSession));

        cout << "Produzindo notificação com ID: " << my_arg_struct->notificationID << endl;
        _this->pendingNotifications.push_back(my_arg_struct->notificationID);

        sem_post(&(_this->freeCritialSession));
        sem_post(&(_this->hasItems));

        cout << "Finalizando thread de produção" <<endl;
        //free(my_arg_struct);
        return 0;

    }

    void *UserInformation::consumer(void *arg) {

        cout << "Iniciando consumo de notificação" << endl;
        UserInformation *_this = (UserInformation *) arg;

        while(_this->getNumberOfSessions()) {
            sleep(rand()%5);
            sem_wait(&(_this->hasItems));
            sem_wait(&(_this->freeCritialSession));

            // this if shouldn't be needed, but better safe than sorry
            if (!_this->pendingNotifications.empty()) {
                string consumedItem = _this->pendingNotifications.front();
                cout << "Consumindo notificação de ID: " << consumedItem << endl;
                _this->pendingNotifications.pop_front();
                GlobalManager::notifManager.sendNotificationTo(_this->username, consumedItem);
            } else {
                cout << "ERRO consumindo notificação" << endl;
            }

            sem_post(&(_this->freeCritialSession));

        }
        cout << "Finalizando thread de consumo" << endl;
        //free(_this);
        return 0;
    }
}