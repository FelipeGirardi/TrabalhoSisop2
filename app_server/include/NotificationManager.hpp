//
// Created by Laura Corssac on 9/9/21.
//

#ifndef NOTIFICATIONMANAGER_HPP
#define NOTIFICATIONMANAGER_HPP

#include "user_information.hpp"
#include "../../Common/include/Notification.hpp"
#include <unordered_map>
#include <string>

using namespace std;
using namespace userInformation;
using namespace notification;

class NotificationManager {

private:
    unordered_map<string,Notification> notifications; // <notification ID> : <notification object>
public:
    /* Initializers */
    NotificationManager();

    /* Getters */
    Notification getNotificationByID(string notificationID);
    unordered_map<string,Notification> getNotifications();

    /* Setters */
    void setNotifications(unordered_map<string,Notification> notifications);

    /* Others */
    /*
     * Method called by consumer thread (Profile Session Manager)
     * when new notification should be sent to user
     *
     */
    static void sendNotificationTo(string username, string notificationID);

    /*
     * Method called by communicator thread when username sends
     * a new notification to its followers
     *
     */
    static void newNotificationSentBy(string username, Notification notification);
};


#endif //NOTIFICATIONMANAGER_HPP
