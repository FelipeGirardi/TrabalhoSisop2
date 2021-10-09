#ifndef SESSION_HPP
#define SESSION_HPP

#include <pthread.h>
#include <stdbool.h>
#include <string>
#include <unordered_map>
//#include "user_information.hpp"

//using namespace userInformation;
using namespace std;

class Session {
    public:
        Session();
        int client_socket;               // Socket do cliente
        int notif_socket;                // Socket das notificações
        string userID;                   // Id do usuário
        bool operator == (const Session& c) const {
            return client_socket == c.client_socket && notif_socket == c.notif_socket;
        }
        bool operator != (const Session& c) const { return !operator==(c); }

        bool hasNotifSocket();
        bool hasCommandSocket();
};

#endif //SESSION_HPP
