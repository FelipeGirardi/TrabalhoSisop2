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

    sessionManager.createNewSession("ufrgs");
    sessionManager.createNewSession("laura");
    sessionManager.addNewFollowerToUser("laura", "ufrgs");

    sleep(5);
    sessionManager.endSession("laura");
    sleep(5);

    int i = 1;
    while (i < 4) {

        sleep(1);
        sessionManager.newNotificationSentBy("ufrgs", to_string(i));
        i++;
    }
    sleep(7);
    sessionManager.createNewSession("laura");
    pthread_exit(0);
    return 0;
}