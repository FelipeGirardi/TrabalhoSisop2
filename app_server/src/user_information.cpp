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
        this->numberOfSessions = 0;
        //this->followers = {};
        sem_init(&(this->freeCritialSession), 0, 1);
        sem_init(&(this->hasItems), 0, 0);
    }

    UserInformation::UserInformation(string username, list<string> pendingNotifications, list <string> followers) {
        this->username = username;
        this->pendingNotifications = pendingNotifications;
        this->followers = followers;
        this->numberOfSessions = 0;
        sem_init(&(this->freeCritialSession), 0, 1);
        sem_init(&(this->hasItems), 0, pendingNotifications.size());
    }

    UserInformation::~UserInformation() { }

    void UserInformation::addNewFollower(string follower) {
        if (find(this->followers.begin(),
                 this->followers.end(), follower) == this->followers.end()) {
            this->followers.push_back(follower);
            return;
        }
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
        fullString += to_string(this->numberOfSessions);
        return fullString;

    }

    void UserInformation::setNumberOfSessions(int numberOfSessions) {
        this->numberOfSessions = numberOfSessions;
    }
    int UserInformation::getNumberOfSessions() {
        return this->numberOfSessions;
    }
    void UserInformation::incrementNumberOfSessions() {
        this->numberOfSessions += 1;
    }
    void UserInformation::decrementNumberOfSessions() {
        this->numberOfSessions -= 1;
    }
    void UserInformation::stopListeningForNotifications() {
        cout << "stop listening for notifications" << endl;
        pthread_cancel(this->consumerTid);
    }

    void UserInformation::produceNewNotification(string notificationID) {

        cout << "producing new notification" << endl;

        cout << "malloc" << endl;
        struct arg_struct *args = new struct arg_struct;

        args->userInformation = this;
        args->notificationID.assign(notificationID);

//        pthread_t tid = (pthread_t *) malloc(sizeof (pthread_t));
//        *tid = (unsigned long int) rand();
        pthread_t tid;

        if (pthread_create(&tid, NULL, this->producer, (void *) args)) {
            cout << "not possible to create producer thread" << endl;
            //free(args);
        }

        // será que é assim?
        //pthread_join(*tid, NULL);
        //free(tid);

    }

    void UserInformation::startListeningForNotifications() {

        if (pthread_create(&(this->consumerTid), NULL, this->consumer, (void *) this)) {
            cout << "not possible to create consumer thread" << endl;
            //free(args);
        }
    }

    void * UserInformation::producer(void *arg) {

        cout << "init producer thread" << endl;

        struct arg_struct *my_arg_struct = (struct arg_struct *) arg;
        UserInformation *_this = my_arg_struct->userInformation;
        sleep(rand()%5);

        sem_wait(&((*_this).freeCritialSession));

        cout << "producing not with ID: " << my_arg_struct->notificationID << endl;
        _this->pendingNotifications.push_back(my_arg_struct->notificationID);

        sem_post(&(_this->freeCritialSession));
        sem_post(&(_this->hasItems));

        cout << "returning from producer thread" <<endl;
        //free(my_arg_struct);
        return 0;

    }

    void *UserInformation::consumer(void *arg) {

        cout << "init consumer thread" << endl;
        UserInformation *_this = (UserInformation *) arg;
        cout << _this->username << endl;
        for (auto v : _this->pendingNotifications) {
            cout << v << " ";
        }
        cout <<endl;
        while(_this->numberOfSessions > 0) {
            cout << "dentro do while" << endl;
            sleep(rand()%5);
            sem_wait(&(_this->hasItems));
            cout << "has items" << endl;
            sem_wait(&(_this->freeCritialSession));
            cout << "SC is free" << endl;

            // this if shouldn't be needed, but better safe than sorry
            if (!_this->pendingNotifications.empty()) {
                string consumedItem = _this->pendingNotifications.front();
                cout << "consuming not with ID: " << consumedItem << endl;
                _this->pendingNotifications.pop_front();
                GlobalManager::notifManager.sendNotificationTo(_this->username, consumedItem);
            } else {
                cout << "not possible to consume" << endl;
            }

            sem_post(&(_this->freeCritialSession));

        }
        cout << "ending consumer thread" << endl;
        //free(_this);
        return 0;
    }
}