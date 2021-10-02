#include <unordered_map>
#include <iostream>
#include <list>
#include <pthread.h>
#include <unistd.h> //close()
#include "../include/profile_session_manager.hpp"
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

    unordered_map<string,UserInformation> ProfileSessionManager::getUsers() {
        return this->users;
    }

    UserInformation ProfileSessionManager::getUserByUsername(string username) {
        return this->users[username];
    }

    void ProfileSessionManager::newNotificationSentBy(string username, string notificationID) {

        list<string> followers = this->users[username].getFollowers();

        for (string follower : followers) {
            if (this->users.find(follower) == this->users.end()) {continue;}
            this->users[follower].produceNewNotification(notificationID);
        }

    }

    void ProfileSessionManager::registerUser(string username) {
        if (GlobalManager::sessionManager.getUsers().find(username) ==
            GlobalManager::sessionManager.getUsers().end()) {
            // user doesn't exist
            cout << "Usuário nao registrado. Criando novo." << endl;
            UserInformation newUserInfo = UserInformation(username);
            this->users[username] = newUserInfo;
        } else {
            // user already registered
            cout << "Usuário já registrado." << endl;
            cout << GlobalManager::sessionManager.getUsers()[username].toString() << endl;
        }
    }

    ErrorCodes ProfileSessionManager::createNewSession(SessionAuth sessionAuth, int socketID) {
        string username = sessionAuth.getProfileId();
        this->registerUser(username);
        if (this->users[username].hasSessionWithID(sessionAuth.getUuid())) {
            cout << "user ja tem sessao com esse id" << endl;
            Session existentSession = this->users[username].getSessionWithID(sessionAuth.getUuid());
            cout << "ex session " << existentSession.client_socket << "  " << existentSession.notif_socket << endl;
            if (sessionAuth.getSocketType() == NOTIFICATION_SOCKET) {
                cout << "NOTIFICATION_SOCKET" << endl;
                if (existentSession.hasNotifSocket()) {
                    cout << "TEM SOCKET DE NOT" << endl;
                    return ERROR;
                } else {
                    this->users[username].updateSession(sessionAuth.getUuid(),
                                                        NOTIFICATION_SOCKET,
                                                        socketID);
                }
            } else if (sessionAuth.getSocketType() == COMMAND_SOCKET ) {
                cout << "COMMAND_SOCKET" << endl;
                if (existentSession.hasCommandSocket()) {
                    cout << "TEM SOCKET DE COMMAND" << endl;
                    return ERROR;
                } else {
                    this->users[username].updateSession(sessionAuth.getUuid(),
                                                        COMMAND_SOCKET,
                                                        socketID);
                }
            } else {
                return ERROR;
            }
        } else if (this->users[username].getNumberOfSessions() < 2) {

            cout << "user ja NAOOO tem sessao com esse id" << endl;
            Session session;
            session.userID = sessionAuth.getProfileId();
            if (sessionAuth.getSocketType() == NOTIFICATION_SOCKET) {
                cout << "NOTIFICATION_SOCKET" << endl;
                session.notif_socket = socketID;
                this->users[username].addNewSession(sessionAuth.getUuid(), session);
            } else if (sessionAuth.getSocketType() == COMMAND_SOCKET) {
                cout << "COMMAND_SOCKET" << endl;
                session.client_socket = socketID;
                this->users[username].addNewSession(sessionAuth.getUuid(), session);
            } else {
                cout << "eh nenhum" << endl;
                return ERROR;
            }

        } else {
            cout << "deu ruim" << endl;
            return ERROR;
        }
        cout << "deu bom" << endl;
        return SUCCESS;
    }

    void ProfileSessionManager::endSessionWithID(string sessionID, string username) {

        cout << "Encerrando sessão de " << username << endl;
        if (this->users.find(username) == this->users.end()) { return; }

        Session session = this->users[username].getSessionWithID(sessionID);
        this->users[username].removeSession(sessionID);

        close(session.notif_socket);
        close(session.client_socket);

        int numberOfSessions = this->users[username].getNumberOfSessions();

        if(numberOfSessions == 0) {
            this->users[username].stopListeningForNotifications();
        }

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