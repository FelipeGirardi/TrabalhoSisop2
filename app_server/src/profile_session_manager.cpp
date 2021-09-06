#include <unordered_map>
#include <iostream>
#include <list>
#include "../include/profile_session_manager.hpp"

using namespace std;
using namespace userInformation;
using namespace notification;

namespace profileSessionManager {

    ProfileSessionManager::ProfileSessionManager() {}

    ProfileSessionManager::ProfileSessionManager(unordered_map <string, UserInformation> users,
                                                 unordered_map<string, Notification> notifications) {

        this->users = users;
        this->notifications = notifications;
    }

    void ProfileSessionManager::setUsers(unordered_map<string, UserInformation> users) {
        this->users = users;
    }

    void ProfileSessionManager::setNotifications(unordered_map<string, Notification> notifications) {
        this->notifications = notifications;
    }

    unordered_map<string,UserInformation> ProfileSessionManager::getUsers() {
        return this->users;
    }

    unordered_map<string,Notification> ProfileSessionManager::getNotifications() {
        return this->notifications;
    }

    void ProfileSessionManager::incrementSessionOfUser(string username) {
        UserInformation userInfo = this->users[username];
        userInfo.incrementNumberOfSessions();
        this->users[username] = userInfo;
    }

    int ProfileSessionManager::getNumberOfSessionsOfUser(string username) {
        return this->users[username].getNumberOfSessions();
    }

}