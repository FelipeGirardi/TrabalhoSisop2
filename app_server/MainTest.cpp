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

int main() {

//    list<string> test_list = {"1","2","3"};
//    list<string> followers_test = {"@renan", "@lize"};
//    Notification notif_test = Notification("1", "ola", "@laura", 29102, 2);
//
//    unordered_map<string, UserInformation> users;
//    unordered_map<string, Notification> notifications;
//
//    notifications["1"] = notif_test;
//
//    users["@mo"] = UserInformation(test_list,followers_test);
//    users["@joana"] = UserInformation(test_list, {});
//    users["@pedrinho"] = UserInformation({},{});
//    users["@yoda"] = UserInformation({},followers_test);
//
//    FileManager fileManager;
//    ProfileSessionManager sessionManager;
//
//    fileManager.saveUsersOnFile(users);
//    fileManager.saveNotificationsOnFile(notifications);
//
//    unordered_map<string,UserInformation> users_retrieval = fileManager.getUsersFromFile();
//    unordered_map<string,Notification> notif_retrieval = fileManager.getNotificationsFromFile();
//    sessionManager.setUsers(users_retrieval);
//    sessionManager.setNotifications(notifications);

//    cout << "USERS" << endl;
//    for (auto user_pair: sessionManager.getUsers()) {
//        cout << user_pair.first << endl;
//        cout << user_pair.second.toString() << endl;
//    }
//    cout << endl << endl;
//    cout << endl << endl;
//    cout << "NOTIFICATIONS" << endl;
//
//    for (auto notif_pair: sessionManager.getNotifications()) {
//        cout << notif_pair.first << endl;
//        cout << notif_pair.second.toString() << endl;
//    }


    cout << "MAIN" << endl;
    ProfileSessionManager sessionManager;
    sessionManager.createNewSession("ufrgs");
    sessionManager.createNewSession("laura");
    sessionManager.addNewFollowerToUser("laura", "ufrgs");
    int i = 0;
    while (i < 3) {

        sleep(1);
        cout << "aaa" << endl;
        sessionManager.newNotificationSentBy("ufrgs", to_string(i));
        cout << "bbb" << endl;
        i++;
    }

    sleep(5);
    cout << "oi" << endl;
    sessionManager.endSession("laura");
    sessionManager.endSession("ufrgs");

//    NotificationManager notificationManager;
//    Notification newNotification = Notification("notID", "oie", "ufrgs", 203912, 0);
//    notificationManager.newNotificationSentBy("ufrgs", newNotification);

    pthread_exit(0);
    return 0;
}