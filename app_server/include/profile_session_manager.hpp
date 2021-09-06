#ifndef PROFILE_SESSION_MANAGER_HPP
#define PROFILE_SESSION_MANAGER_HPP

#include "user_information.hpp"

#include <unordered_map>
#include <string>
#include <iostream>

using namespace std;
using namespace userInformation;

namespace profileSessionManager {

    class ProfileSessionManager {
    private:
        unordered_map<string,UserInformation> users; // <username> : <user info object>
        //TODO: change type of value to the one of notification
        unordered_map<string,string> notifications; // <notification ID> : <notification object>
    public:
        /* Initializers */
        ProfileSessionManager();
        ProfileSessionManager(unordered_map<string, UserInformation> users, unordered_map<string, string>notifications);

        /* Setters */
        void setUsers(unordered_map<string,UserInformation> users);

        /* Getters */
        unordered_map<string,UserInformation> getUsers();
        unordered_map<string,string> getNotifications();
    };
}
#endif