#include <unordered_map>
#include <iostream>
#include <list>
#include <string>
#include "../include/profile_session_manager.hpp"
#include "../include/user_information.hpp"

using namespace std;
using namespace userInformation;

namespace profileSessionManager {

    ProfileSessionManager::ProfileSessionManager() {}

    ProfileSessionManager::ProfileSessionManager(unordered_map <string, UserInformation> users,
                                                 unordered_map<string, string> notifications) {

        this->users = users;
        this->notifications = notifications;
    }

    void ProfileSessionManager::setUsers(unordered_map<string, UserInformation> users) {
        this->users = users;
    }

    unordered_map<string,UserInformation> ProfileSessionManager::getUsers() {
        return this->users;
    }

    unordered_map<string,string> ProfileSessionManager::getNotifications() {
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