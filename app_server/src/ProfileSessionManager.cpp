#include <unordered_map>
#include <iostream>
#include <list>
#include <pthread.h>
#include <unistd.h> //close()
#include "../include/ProfileSessionManager.hpp"
#include "../include/GlobalManager.hpp"
#include "../include/utils/ErrorCodes.hpp"
#include "../../common/include/SessionAuth.hpp"

using namespace std;
using namespace userInformation;
using namespace notification;
using namespace Common;

namespace profileSessionManager {

    ProfileSessionManager::ProfileSessionManager() {}

    ProfileSessionManager::ProfileSessionManager(unordered_map <string, UserInformation> users) {

        this->users = users;
    }

    void ProfileSessionManager::setUsers(unordered_map<string, UserInformation> users) {
        this->users = users;
    }

    unordered_map<string, UserInformation> ProfileSessionManager::getUsers() {
        return this->users;
    }

    UserInformation ProfileSessionManager::getUserByUsername(string username) {
        return this->users[username];
    }

    void ProfileSessionManager::newNotificationSentBy(string username, string notificationID) {

        list<string> followers = this->users[username].getFollowers();

        for (string follower : followers) {
            if (GlobalManager::sessionManager.users.find(follower) == GlobalManager::sessionManager.users.end()) { continue; }
            GlobalManager::sessionManager.users[follower].produceNewNotification(notificationID);
        }

    }

    void ProfileSessionManager::registerUser(string username) {
        if (GlobalManager::sessionManager.getUsers().find(username) ==
            GlobalManager::sessionManager.getUsers().end()) {
            // user doesn't exist
            cout << "Usuário nao registrado. Criando novo." << endl;
            UserInformation newUserInfo = UserInformation(username);
            GlobalManager::sessionManager.users[username] = newUserInfo;
        }
        else {
            // user already registered
            cout << "Usuário já registrado." << endl;
            cout << GlobalManager::sessionManager.getUsers()[username].toString() << endl;
        }
    }

    ErrorCodes ProfileSessionManager::createNewSession(string username, string sessionID) {
        GlobalManager::sessionManager.registerUser(username);

        if (GlobalManager::sessionManager.users[username].getNumberOfSessions() < 2) {

            Session session;
            session.userID = username;
            GlobalManager::sessionManager.users[username].addNewSession(sessionID,
                session);

            return SUCCESS;

        }
        else {
            return ERROR;
        }

    }

    void ProfileSessionManager::endSessionWithID(string sessionID, string username) {

        cout << "Encerrando sessão " << sessionID << " de " << username << endl;
        if (GlobalManager::sessionManager.users.find(username) == GlobalManager::sessionManager.users.end()) {
            cout << "ERRO encerrando sessão. Usuário não existe." << endl;
            return;
        }

        if (!GlobalManager::sessionManager.users[username].hasSessionWithID(sessionID)) {
            cout << "ERRO encerrando sessão. Sessão não existe." << endl;
            return;
        }

        Session session = GlobalManager::sessionManager.users[username].getSessionWithID(sessionID);
        cout << "Encontrei a sessão de " << session.userID << endl;
        GlobalManager::sessionManager.users[username].removeSession(sessionID);

    }

    // Só chamada pelo RM primário
    void ProfileSessionManager::additionalSessionClosingProcedure(string username) {

        int numberOfSessions = GlobalManager::sessionManager.users[username].getNumberOfSessions();
        cout << "Número de sessões desse usuário = " << numberOfSessions;

        if (numberOfSessions == 0) {
            cout << "Encerrando thread de escuta de notificações" << endl;
            GlobalManager::sessionManager.users[username].stopListeningForNotifications();
        }
    }

    // Só chamada pelo RM primário
    void ProfileSessionManager::additionalSessionOpeningProcedure(string username) {

        int numberOfSessions = GlobalManager::sessionManager
            .getUserByUsername(username).getNumberOfSessions();
        cout << "Número de sessões do usuário = " << username << " = " << numberOfSessions;

        if (numberOfSessions >= 1) {
            cout << "Iniciando thread de escuta de notificações do usuario " << username << endl;
            GlobalManager::sessionManager.users[username]
                .startListeningForNotifications();
        }
    }

    /*
     * Retorno:
     * 1 = deu follow com sucesso
     * -1 = nao deu follow
    */
    ErrorCodes ProfileSessionManager::addNewFollowerToUser(string follower, string toBeFollowed) {
        cout << "Adicionando novo seguidor " << follower << " ao usuário " << toBeFollowed << endl;

        if (follower.compare(toBeFollowed) == 0) {
            cout << "Não é possível seguir a si mesmo. Seu narcisista." << endl;
            return ERROR;
        }

        if (GlobalManager::sessionManager.users.find(toBeFollowed) == GlobalManager::sessionManager.users.end()) {
            cout << "Não foi possível seguir " << toBeFollowed << " pois ele não existe." << endl;
            return ERROR;
        }

        return GlobalManager::sessionManager.users[toBeFollowed].addNewFollower(follower);

    }

    void ProfileSessionManager::endAllSessions() {
        cout << "Encerrando todas as sessões de todos os usuários." << endl;
        for (auto user : GlobalManager::sessionManager.users) {

            cout << "Usuário = " << user.first << endl;
            for (auto kv : user.second.getSessions()) {
                cout << "ID da Sessão = " << kv.first << endl;
                GlobalManager::sessionManager.endSessionWithID(kv.first,
                    user.first);
            }
            cout << endl;
        }
    }

    list<Session> ProfileSessionManager::getAllSessions() {
        list<Session> returnValue;
        for (auto user : GlobalManager::sessionManager.users) {
            for (auto kv : user.second.getSessions()) {
                returnValue.push_back(kv.second);
            }
        }
        return returnValue;
    }

    void ProfileSessionManager::deleteNotificationFromUser(string username, string notificationID) {
        this->users[username].deletePendingNotification(notificationID);
    }

}
