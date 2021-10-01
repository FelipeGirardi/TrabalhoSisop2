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

        list<string> followers = this->users[username].getFollowers();

        for (string follower : followers) {
            if (this->users.find(follower) == this->users.end()) {continue;}
            this->users[follower].produceNewNotification(notificationID);
        }

    }

    /*
     * Retorno:
     * 1 = criou sessao com sucesso
     * -1 = nao criou
    */
    int ProfileSessionManager::createNewSession(Session session) {

        // check if user exists
        string username = session.userID;

        if (GlobalManager::sessionManager.getUsers().find(username) ==
        GlobalManager::sessionManager.getUsers().end()) {
            // user doesn't exist
            cout << "Usuário nao existe no banco de dados. Criando novo." << endl;
            UserInformation newUserInfo = UserInformation(username);
            this->users[username] = newUserInfo;

        } else {
            // user already registered
            cout << "Usuário encontrado no banco." << endl;
        }

        int currentNumberSessions = this->users[username].getNumberOfSessions();
        if (currentNumberSessions >= 2) { return -1; }

        // Number of sessions of user is 0
        if (currentNumberSessions == 0) {
            this->users[username].startListeningForNotifications();
        }

        // Success on creating new session for user
        this->users[username].sessions.push_back(session);
        this->users[username].incrementNumberOfSessions();
        return 1;

    }

    void ProfileSessionManager::endSession(Session session) {

        cout << "Encerrando sessão de " << session.userID << endl;
        if (this->users.find(session.userID) == this->users.end()) { return; }

        int numberOfSessions = this->users[session.userID].getNumberOfSessions();

        if ( numberOfSessions == 2) {
            this->users[session.userID].decrementNumberOfSessions();
        } else if(numberOfSessions == 1) {
            this->users[session.userID].stopListeningForNotifications();
            this->users[session.userID].decrementNumberOfSessions();
        }
        this->users[session.userID].sessions.remove(session);

        close(session.notif_socket);
        close(session.client_socket);

    }

    /*
     * Retorno:
     * 1 = deu follow com sucesso
     * -1 = nao deu follow
    */
    int ProfileSessionManager::addNewFollowerToUser(string follower, string toBeFollowed) {
        cout << "Adicionando novo seguidor " << follower << "ao usuário " << toBeFollowed << " " << endl;

        if (follower.compare(toBeFollowed) == 0) {
            cout << "Não é possível seguir a si mesmo. Seu narcisista." << endl;
            return -1;
        }

        if (this->users.find(toBeFollowed) == this->users.end()) {
            cout << "Não foi possível seguir " << toBeFollowed << " pois ele não existe." << endl;
            return -1;
        }

        return this->users[toBeFollowed].addNewFollower(follower);

    }


}