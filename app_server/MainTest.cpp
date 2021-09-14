//
// Created by Laura Corssac on 9/5/21.
//

#include <iostream>
#include <unordered_map>
#include <list>
#include <unistd.h>
#include <pthread.h>
#include "include/profile_session_manager.hpp"
#include "include/user_information.hpp"
#include "include/FileManager.hpp"
#include "../Common/include/Notification.hpp"
#include "include/NotificationManager.hpp"

using namespace std;
using namespace profileSessionManager;
using namespace userInformation;
using namespace notification;

unordered_map<string,UserInformation> users;
ProfileSessionManager sessionManager;

int main() {

    cout << "MAIN" << endl;

    FileManager fileManager;
    users = fileManager.getUsersFromFile();
    sessionManager.setUsers(users);

    sessionManager.createNewSession("@joana");
    sessionManager.createNewSession("@moritz");
    sessionManager.newNotificationSentBy("@moritz", "6");

    pthread_exit(0);
    return 0;
}