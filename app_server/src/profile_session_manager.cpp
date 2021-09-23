#include <unordered_map>
#include <iostream>
#include <list>
#include <pthread.h>
#include <unistd.h> //close()
#include "../include/profile_session_manager.hpp"
#include "../include/GlobalManager.hpp"

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

        cout << "newNotificationSentBy " << username << " " << " " << endl;
        cout << "users" << endl << endl;

        list<string> followers = this->users[username].getFollowers();
        cout << "followers size " << followers.size() << endl;
        for (string follower : followers) {
            cout << "follower" << follower;
            if (this->users.find(follower) == this->users.end()) {continue;}
            this->users[follower].produceNewNotification(notificationID);
        }

    }

    /*
     * Retorno:
     * 1 = criou sessao com sucesso
     * -1 = nao criou
    */
    int ProfileSessionManager::createNewSession(string username, Session session) {

        // check if user exists
        if (GlobalManager::sessionManager.getUsers().find(username) ==
        GlobalManager::sessionManager.getUsers().end()) {
            // user doesn't exist
            cout << "user doesnt exist" << endl;
            UserInformation newUserInfo = UserInformation(username);
            this->users[username] = newUserInfo;

        } else {
            cout << "user already exists" << endl;
        }

        int currentNumberSessions = this->users[username].getNumberOfSessions();
        if (currentNumberSessions >= 2) { return -1; }
        if (currentNumberSessions == 0) {
            cout << "number of sessions of user is 0" << endl;
            this->users[username].startListeningForNotifications();
        }

        // Success on creating new session for user
        this->users[username].sessions.push_back(session);
        this->users[username].incrementNumberOfSessions();
        return 1;

    }

    void ProfileSessionManager::endSession(Session session) {

        if (this->users.find(session.userID) == this->users.end()) { return; }

        int numberOfSessions = this->users[session.userID].getNumberOfSessions();

        if ( numberOfSessions == 2) {
            cout << "number of sessions of user is 2" << endl;
            this->users[session.userID].decrementNumberOfSessions();
        } else if(numberOfSessions == 1) {
            cout << "number of sessions of user is 1" << endl;
            this->users[session.userID].stopListeningForNotifications();
            this->users[session.userID].decrementNumberOfSessions();
        }
        this->users[session.userID].sessions.remove(session);

        cout << this->users[session.userID].username << " sessions after remove:" << endl;
        for (Session sess : this->users[session.userID].sessions)
            cout << sess.client_socket << ' ' << sess.notif_socket << ' ' << sess.userID << endl;

        cout << "now the number of sessions is " << this->users[session.userID].getNumberOfSessions() << endl;
        cout << "and the length of sessions is " << this->users[session.userID].sessions.size() << endl;
        close(session.notif_socket);
        close(session.client_socket);

    }

    void ProfileSessionManager::addNewFollowerToUser(string follower, string toBeFollowed) {
        cout << "addNewFollowerToUser " << follower << " " << toBeFollowed << " " << endl;
        if (this->users.find(toBeFollowed) == this->users.end()) { return; }
        this->users[toBeFollowed].addNewFollower(follower);
    }


}