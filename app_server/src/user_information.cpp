#include "../include/user_information.hpp"
//#include <semaphore.h>

namespace userInformation {

    UserInformation::UserInformation() {}
    UserInformation::UserInformation(list<string> pendingNotifications, list <string> followers) {
//        sem_init(&this->mutexP, 0, 1);
//        sem_init(&this->mutexC, 0, 1);
//        sem_init(&this->hasItemsToConsume, 0, 0);
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

    void UserInformation::addNewNotification(string notificationID) {
        this->pendingNotifications.push_back(notificationID);
    }
    void UserInformation::addNewNotifications(list<string> notificationsIDs) {
        for (string notification: notificationsIDs) {
            this->addNewNotification(notification);
        }
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

    /* semaphore methods */
//    void *produce(void *arg) {
//
//        sem_wait(&this->mutexP);
//        string *notificationID = static_cast<string *>(arg);
//        cout << "PRODUCING" << notificationID << endl;
//        this->pendingNotifications.push_back(notificationID);
//        sem_post(&this->hasItemsToConsume);
//        sem_post(&this->mutexP);
//    }
//    void *consume(void *arg) {
//
//        sem_wait(&this->hasItemsToConsume);
//        sem_wait(&this->mutexC);
//        string *notificationID = static_cast<string *>(arg);
//        cout << "PRODUCING" << notificationID << endl;
//        this->pendingNotifications.push_back(notificationID);
//        sem_post(&this->mutexC);
//    }

}