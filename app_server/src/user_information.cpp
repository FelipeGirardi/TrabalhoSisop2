#include "../include/user_information.hpp"
#include "../include/NotificationManager.hpp"
#include <semaphore.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//TODO: move this from here
struct arg_struct {
    userInformation::UserInformation *userInformation;
    string notificationID;
};

namespace userInformation {

    UserInformation::UserInformation() {}
    UserInformation::UserInformation(string username, list<string> pendingNotifications, list <string> followers) {
        this->username = username;
        this->pendingNotifications = pendingNotifications;
        this->followers = followers;
    }
    void UserInformation::addNewFollower(string follower) {
        this->followers.push_back(follower);
    }
    void UserInformation::addNewFollowers(list <string> followers) {
        for (string follower : followers) {
            this->addNewFollower(follower);
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
        fullString += "Followers:\n";
        for (string follower : this->followers) {
            fullString += follower + " ";
        }
        fullString += '\n';
        fullString += "Notifications:\n";
        for (string notification : this->pendingNotifications) {
            fullString += notification + " ";
        }
        fullString += '\n';
        fullString += "NumberOfSessions:\n";
        fullString += to_string(this->numerOfSessions);
        return fullString;

    }

    void UserInformation::setNumberOfSessions(int numberOfSessions) {
        this->numerOfSessions = numberOfSessions;
    }
    int UserInformation::getNumberOfSessions() {
        return this->numerOfSessions;
    }
    void UserInformation::incrementNumberOfSessions() {
        this->numerOfSessions += 1;
    }
    void UserInformation::decrementNumberOfSessions() {
        this->numerOfSessions -= 1;
    }

    void UserInformation::produceNewNotification(string notificationID) {

        struct arg_struct my_args;
        my_args.userInformation = this;
        my_args.notificationID = notificationID;

        struct arg_struct *args = (struct arg_struct *) malloc(sizeof(struct arg_struct));
        *args = my_args;

        pthread_create(&(this->tid), NULL, this->producer, (void *) args);
    }

    void UserInformation::startListeningForNotifications() {

        UserInformation *args = (UserInformation *) malloc(sizeof(UserInformation *));
        args = this;
        pthread_create(&(this->consumerTid), NULL, this->consumer, (void *) args);
    }

    void * UserInformation::producer(void *arg) {

        cout << "init producer thread" << endl;
        struct arg_struct *my_arg_struct = (struct arg_struct *) arg;
        UserInformation *_this = my_arg_struct->userInformation;
        sleep(rand()%5);

        sem_wait(&((*_this).freeCritialSession));

        cout << "producing: " << my_arg_struct->notificationID << endl;
        _this->pendingNotifications.push_back(my_arg_struct->notificationID);

        sem_post(&(_this->freeCritialSession));
        sem_post(&(_this->hasItems));

        free(arg);
        cout << "returning from producer" <<endl;
        return 0;

    }

    void *UserInformation::consumer(void *arg) {

        cout << "init consumer thread" << endl;
        UserInformation *_this = (UserInformation *) arg;

        while(_this->numerOfSessions > 0) {
            cout << "dentro do while" << endl;
            sleep(rand()%5);
            sem_wait(&((*_this).hasItems));
            cout << "has items" << endl;
            sem_wait(&((*_this).freeCritialSession));
            cout << "SC is free" << endl;

            // this if shouldn't be needed, but better safe than sorry
            if (!_this->pendingNotifications.empty()) {
                string consumedItem = _this->pendingNotifications.front();
                cout << "consuming: " << consumedItem << endl;
                _this->pendingNotifications.pop_front();
                NotificationManager::sendNotificationTo(_this->username, consumedItem);
            } else {
                cout << "not possible to consume" << endl;
            }

            sem_post(&(_this->freeCritialSession));

        }

        free(arg);
        return 0;
    }
}