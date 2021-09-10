#include <unordered_map>
#include <iostream>
#include <list>
#include <pthread.h>
#include "../include/profile_session_manager.hpp"

using namespace std;
using namespace userInformation;
using namespace notification;

namespace profileSessionManager {

    ProfileSessionManager::ProfileSessionManager() {}

    ProfileSessionManager::ProfileSessionManager(unordered_map <string, UserInformation> users) {

        this->users = users;
    }

    void ProfileSessionManager::setUsers(unordered_map<string, UserInformation> users) {
        this->users = users;
    }

    unordered_map<string,UserInformation> ProfileSessionManager::getUsers() {
        return this->users;
    }

    UserInformation ProfileSessionManager::getUserByUsername(string username) {
        return this->users[username];
    }

    void ProfileSessionManager::incrementSessionOfUser(string username) {
        UserInformation userInfo = this->users[username];
        userInfo.incrementNumberOfSessions();
        this->users[username] = userInfo;
    }

    int ProfileSessionManager::getNumberOfSessionsOfUser(string username) {
        return this->users[username].getNumberOfSessions();
    }

    void ProfileSessionManager::newNotificationSentBy(string username, string notificationID) {

        list<string> followers = this->users[username].getFollowers();
        for (string follower : followers) {
            this->users[follower].produceNewNotification(notificationID);
        }
        pthread_exit(NULL);
    }

    void ProfileSessionManager::createNewSession(string username) {

    }


}