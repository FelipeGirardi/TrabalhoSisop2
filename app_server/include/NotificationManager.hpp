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
        int idNextNotification;
        unordered_map <string, Notification> notifications; // <notification ID> : <notification object>

        int getPendingReaders(string username);
    public:
        /* Initializers */
        NotificationManager();

        /* Getters */
        Notification getNotificationByID(string notificationID);

        unordered_map <string, Notification> getNotifications();

        /* Setters */
        void setNotifications(unordered_map <string, Notification> notifications);
        void addNewNotification(Notification notification);

        /* Others */
        /*
         * Method called by consumer thread (Profile Session Manager)
         * when new notification should be sent to user
         *
         */
        void sendNotificationTo(string username, string notificationID);

        /*
         * Method called by communicator thread when username sends
         * a new notification to its followers. Creates notification object
         *
         */
        void newNotificationSentBy(string username, string notification);

};


#endif //NOTIFICATIONMANAGER_HPP
