#ifndef PROFILE_SESSION_MANAGER_HPP
#define PROFILE_SESSION_MANAGER_HPP

#include "user_information.hpp"
#include "../../common/include/Notification.hpp"

#include <unordered_map>
#include <string>
#include <iostream>

using namespace std;
using namespace userInformation;
using namespace notification;

namespace profileSessionManager {

    class ProfileSessionManager {
    private:
        unordered_map<string, UserInformation> users; // <username> : <user info object>
    public:
        /* Initializers */
        ProfileSessionManager();
        ProfileSessionManager(unordered_map<string, UserInformation> users);

        /* Setters */
        void setUsers(unordered_map<string, UserInformation> users);

        /* Getters */
        unordered_map<string, UserInformation> getUsers();
        UserInformation getUserByUsername(string username);

        /* Others */
        void incrementSessionOfUser(string username);
        int getNumberOfSessionsOfUser(string username);

        /*
         * Method called by Notification Manager
         * Creates producer thread
         *
         * @Param username: the user who sent the notification
         * @Param notificationID: the ID of notification to be sent to all followers of `username`
         */
        static void newNotificationSentBy(string username, string notificationID);

        /*
        * Method called by Communication Manager
        * Creates consumer thread
        *
        * @Param username: the user who wants to login/create account
        *
        */
        static void createNewSession(string username);

    };
}
#endif
