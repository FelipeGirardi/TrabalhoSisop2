#include "../include/user_information.hpp"
#include <sstream>
#include <iostream>

namespace userInformation {

UserInformation::UserInformation() {}
UserInformation::UserInformation(list<int> pendingNotifications, list <string> followers) {
    this->pendingNotifications = pendingNotifications;
    this->followers = followers;
}
void UserInformation::addNewFollower(string follower) {
    this->followers.push_back(follower);
}
void UserInformation::addNewFollowers(list <string> followers) {
    for (string follower : followers) {
        this->addNewFollower(follower);
    }
}
void UserInformation::setFollowers(list <string> followers) {
    this->followers = followers;
}

void UserInformation::addNewNotification(int notificationID) {
    this->pendingNotifications.push_back(notificationID);
}
void UserInformation::addNewNotifications(list<int> notificationsIDs) {
    for (int notification: notificationsIDs) {
        this->addNewNotification(notification);
    }
}
void UserInformation::setNotifications(list<int> notificationsIDs) {
    this->pendingNotifications = notificationsIDs;
}

}