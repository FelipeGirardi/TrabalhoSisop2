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
#include "include/GlobalManager.hpp"

using namespace std;
using namespace profileSessionManager;
using namespace userInformation;
using namespace notification;

ProfileSessionManager sessionManager;
NotificationManager notificationManager;
GlobalManager globalManager;
FileManager fileManager;

int main() {

    cout << "MAIN" << endl;

    unordered_map<string,UserInformation> users = fileManager.getUsersFromFile();

    GlobalManager::sessionManager = sessionManager;
    GlobalManager::notifManager = notificationManager;
    GlobalManager::sessionManager.setUsers(users);


    GlobalManager::sessionManager.createNewSession("@joana");
    GlobalManager::sessionManager.createNewSession("@moritz");
    //GlobalManager::sessionManager.newNotificationSentBy("@moritz", "10");

    sleep(2);

    GlobalManager::sessionManager.endSession("@joana");

    sleep(2);

    //GlobalManager::sessionManager.newNotificationSentBy("@moritz", "22");
   // sessionManager.addNewFollowerToUser("@moritz", "@joana");

    sleep(2);

    GlobalManager::sessionManager.createNewSession("@joana");
    sleep(2);
    GlobalManager::sessionManager.newNotificationSentBy("@joana", "44");

    pthread_exit(0);
    return 0;
}
