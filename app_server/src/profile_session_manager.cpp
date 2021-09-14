#include <unordered_map>
#include <iostream>
#include <list>
#include <pthread.h>
#include "../include/profile_session_manager.hpp"

using namespace std;
using namespace userInformation;
using namespace notification;

namespace profileSessionManager {

    ProfileSessionManager::ProfileSessionManager() {}

    ProfileSessionManager::ProfileSessionManager(unordered_map <string, UserInformation> users) {

        this->users = users;
    }

    void ProfileSessionManager::setUsers(unordered_map<string, UserInformation> users) {
        this->users = users;
    }

    unordered_map<string,UserInformation> ProfileSessionManager::getUsers() {
        return this->users;
    }

    UserInformation ProfileSessionManager::getUserByUsername(string username) {
        return this->users[username];
    }

    void ProfileSessionManager::incrementSessionOfUser(string username) {
        UserInformation userInfo = this->users[username];
        userInfo.incrementNumberOfSessions();
        this->users[username] = userInfo;
    }

    int ProfileSessionManager::getNumberOfSessionsOfUser(string username) {
        return this->users[username].getNumberOfSessions();
    }

    void ProfileSessionManager::newNotificationSentBy(string username, string notificationID) {

        cout << "newNotificationSentBy" << username << " " << " " << endl;
        list<string> followers = this->users[username].getFollowers();
        for (string follower : followers) {
            if (this->users.find(follower) == this->users.end()) {continue;}
            this->users[follower].produceNewNotification(notificationID);
        }
        //pthread_join();
    }

    void ProfileSessionManager::createNewSession(string username) {

        // check if user exists
        if (this->users.find(username) == this->users.end()) {
            // user doesn't exist
            cout << "user doesnt exist" << endl;
            UserInformation newUserInfo = UserInformation(username);
            this->users[username] = newUserInfo;
        }

        int currentNumberSessions = this->users[username].getNumberOfSessions();
        if (currentNumberSessions >= 2) { return; }
        if (currentNumberSessions == 0) {
            cout << "number of sessions of user is 0" << endl;
            this->users[username].startListeningForNotifications();
        }
        this->users[username].incrementNumberOfSessions();

    }

    void ProfileSessionManager::endSession(string username) {

        if (this->users.find(username) == this->users.end()) { return; }

        int numberOfSessions = this->users[username].getNumberOfSessions();

        if ( numberOfSessions == 2) {
            cout << "number of sessions of user is 2" << endl;
            this->users[username].decrementNumberOfSessions();
        } else if(numberOfSessions == 1) {
            cout << "number of sessions of user is 1" << endl;
            this->users[username].stopListeningForNotifications();
            this->users[username].decrementNumberOfSessions();
        }

    }

    void ProfileSessionManager::addNewFollowerToUser(string follower, string toBeFollowed) {
        cout << "addNewFollowerToUser " << follower << " " << toBeFollowed << " " << endl;
        if (this->users.find(toBeFollowed) == this->users.end()) { return; }
        this->users[toBeFollowed].addNewFollower(follower);
    }


}