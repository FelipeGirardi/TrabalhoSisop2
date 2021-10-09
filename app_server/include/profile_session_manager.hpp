#ifndef PROFILE_SESSION_MANAGER_HPP
#define PROFILE_SESSION_MANAGER_HPP

#include "user_information.hpp"
#include "../../Common/include/Notification.hpp"
#include "../../Common/include/SessionAuth.hpp"
#include "../include/utils/ErrorCodes.hpp"

#include <unordered_map>
#include <string>
#include <iostream>

using namespace Common;
using namespace std;
using namespace userInformation;
using namespace notification;

namespace profileSessionManager {

    class ProfileSessionManager {
    private:

        void registerUser(string username);
    public:
        unordered_map<string, UserInformation> users; // <username> : <user info object>
        /* Initializers */
        ProfileSessionManager();
        ProfileSessionManager(unordered_map<string, UserInformation> users);

        /* Setters */
        void setUsers(unordered_map<string, UserInformation> users);

        /* Getters */
        unordered_map<string, UserInformation> getUsers();
        UserInformation getUserByUsername(string username);

        /*
         * Method called by Notification Manager
         *
         * @Param username: the user who sent the notification
         * @Param notificationID: the ID of notification to be sent to all followers of `username`
         */
        void newNotificationSentBy(string username, string notificationID);

        /*
        * Method called by Communication Manager
        * It decrements the number of sessions of user
         * Collateral effect: if it is the only session of user, consumer thread is ended.
        *
        * @Param sessionID: the ID session or the username to be deleted
        * @Param username: the username of the user
        */
        void endSessionWithID(string sessionID, string username);

        /**
         * Adds a new username (follower) to the list of followers of an user (toBeFollowed)
         *
         * @param follower - username of new user to be added to the list
         * @param toBeFollowed  - username the user whose list is gonna be changed
         *
         */
        ErrorCodes addNewFollowerToUser(string follower, string toBeFollowed);
        ErrorCodes createNewSession(SessionAuth sessionAuth, int socketID);

        void endAllSessions();
        list<Session> getAllSessions();


        };
}
#endif
