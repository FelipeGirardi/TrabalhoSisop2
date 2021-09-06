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

    list<string> test_list = {"1","2","3"};
    list<string> followers_test = {"@renan", "@lize"};

    unordered_map<string, UserInformation> users;
    unordered_map<string, string> notifications;
    users["@mo"] = UserInformation(test_list,followers_test);
    users["@joana"] = UserInformation(test_list, {});
    users["@pedrinho"] = UserInformation({},{});
    users["@yoda"] = UserInformation({},followers_test);


    ProfileSessionManager sessionManager = ProfileSessionManager(users,notifications);

    sessionManager.saveUsersOnFile();
    unordered_map<string,UserInformation> users_retrieval = sessionManager.getUsersFromFile();
    sessionManager.setUsers(users_retrieval);

    for (auto user_pair: sessionManager.getUsers()) {
        cout << user_pair.first << endl;
        cout << user_pair.second.toString() << endl;
    }


    return 0;
}