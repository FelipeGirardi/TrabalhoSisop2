//
// Created by Laura Corssac on 11/18/21.
//

#ifndef USERINFOPAYLOAD_H
#define USERINFOPAYLOAD_H

class UserInfoPayload {
    char username[100];
    int pendingNotifications[100];
    char followers[];
};

#endif //USERINFOPAYLOAD_HPP
