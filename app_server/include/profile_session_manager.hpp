#ifndef PROFILE_SESSION_MANAGER_HPP
#define PROFILE_SESSION_MANAGER_HPP

#include "user_information.hpp"
#include "../../Common/include/Notification.hpp"

#include <unordered_map>
#include <string>
#include <iostream>

using namespace std;
using namespace userInformation;
using namespace notification;

namespace profileSessionManager {

    class ProfileSessionManager {
    private:
        unordered_map<string,UserInformation> users; // <username> : <user info object>
        unordered_map<string,Notification> notifications; // <notification ID> : <notification object>
    public:
        /* Initializers */
        ProfileSessionManager();
        ProfileSessionManager(unordered_map<string, UserInformation> users, unordered_map<string, Notification>notifications);

        /* Setters */
        void setUsers(unordered_map<string,UserInformation> users);
        void setNotifications(unordered_map<string,Notification> notifications);

        /* Getters */
        unordered_map<string,UserInformation> getUsers();
        unordered_map<string,Notification> getNotifications();

        /* Others */
        void incrementSessionOfUser(string username);
        int getNumberOfSessionsOfUser(string username);


    };
}
#endif