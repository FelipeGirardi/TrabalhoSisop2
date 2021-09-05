//
// Created by Laura Corssac on 9/5/21.
//

#include <iostream>
#include <unordered_map>
#include <list>
#include "include/profile_session_manager.hpp"
#include "include/user_information.hpp"

using namespace std;
using namespace profileSessionManager;
using namespace userInformation;
unordered_map<string,UserInformation> users;

int main() {

    list<int> test_list = {1,2,3};
    list<string> followers_test = {"@renan", "@lize"};

    unordered_map<string, UserInformation> users;
    unordered_map<int, string> notifications;
    users["@felipe"] = UserInformation(test_list,followers_test);
    users["@laura"] = UserInformation(test_list, {});
    users["@lize"] = UserInformation({},{});
    users["@renan"] = UserInformation({},followers_test);


    ProfileSessionManager sessionManager = ProfileSessionManager(users,notifications);

    sessionManager.saveUsersOnFile();
    sessionManager.getUsersFromFile();
    return 0;
}