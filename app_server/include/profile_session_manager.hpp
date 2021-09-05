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
        unordered_map<string,UserInformation> users;
        //TODO: change type of value to the one of notification
        unordered_map<int,string> notifications;
    public:
        ProfileSessionManager(unordered_map<string, UserInformation> users, unordered_map<int, string>notifications);
        unordered_map<string,UserInformation> getUsersFromFile();
        void saveUsersOnFile();
    };
}
#endif