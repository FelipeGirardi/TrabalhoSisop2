#include "../include/user_information.hpp"

namespace userInformation {

    UserInformation::UserInformation() {}
    UserInformation::UserInformation(list<string> pendingNotifications, list <string> followers) {
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

    void UserInformation::addNewNotification(string notificationID) {
        this->pendingNotifications.push_back(notificationID);
    }
    void UserInformation::addNewNotifications(list<string> notificationsIDs) {
        for (string notification: notificationsIDs) {
            this->addNewNotification(notification);
        }
    }
    void UserInformation::setNotifications(list<string> notificationsIDs) {
        this->pendingNotifications = notificationsIDs;
    }

    list<string> UserInformation::getFollowers() {
        return this->followers;
    }

    list<string> UserInformation::getPendingNotifications() {
        return this->pendingNotifications;
    }

    string UserInformation::toString() {
        string fullString;
        fullString += "Followers:\n";
        for (string follower : this->followers) {
            fullString += follower + " ";
        }
        fullString += '\n';
        fullString += "Notifications:\n";
        for (string notification : this->pendingNotifications) {
            fullString += notification + " ";
        }
        fullString += '\n';
        fullString += "NumberOfSessions:\n";
        fullString += to_string(this->numerOfSessions);
        return fullString;

    }

    void UserInformation::setNumberOfSessions(int numberOfSessions) {
        this->numerOfSessions = numberOfSessions;
    }
    int UserInformation::getNumberOfSessions() {
        return this->numerOfSessions;
    }
    void UserInformation::incrementNumberOfSessions() {
        this->numerOfSessions += 1;
    }

}