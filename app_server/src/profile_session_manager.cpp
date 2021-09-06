#include <unordered_map>
#include <iostream>
#include <list>
#include "../include/profile_session_manager.hpp"

using namespace std;

namespace profileSessionManager {

    ProfileSessionManager::ProfileSessionManager() {}

    ProfileSessionManager::ProfileSessionManager(unordered_map <string, UserInformation> users,
                                                 unordered_map<string, string> notifications) {

        this->users = users;
        this->notifications = notifications;
    }

    void ProfileSessionManager::setUsers(unordered_map<string,UserInformation> users) {
        this->users = users;
    }

    unordered_map<string,UserInformation> ProfileSessionManager::getUsers() {
        return this->users;
    }

    unordered_map<string,string> ProfileSessionManager::getNotifications() {
        return this->notifications;
    }

}