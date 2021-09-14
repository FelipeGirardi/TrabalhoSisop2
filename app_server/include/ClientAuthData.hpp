#ifndef TRABALHOSISOP2_CLIENTAUTHDATA_HPP
#define TRABALHOSISOP2_CLIENTAUTHDATA_HPP

#include <pthread.h>
#include <stdbool.h>
#include <string>
#include <unordered_map>
#include "user_information.hpp"

using namespace userInformation;

class ClientAuthData {
    public:
        unordered_map<string, UserInformation> users; // <username> : <user info object>
        int client_sockfd;               // Socket do cliente
        int notif_sockfd;                // Socket das notificações
        int userID;                      // Id do usuário (TALVEZ O ID TENHA QUE SER INT EM VEZ DE STRING)
};

#endif //TRABALHOSISOP2_CLIENTAUTHDATA_HPP
