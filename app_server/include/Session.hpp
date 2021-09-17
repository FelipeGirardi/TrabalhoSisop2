//
// Created by Laura Corssac on 9/14/21.
//

#ifndef SESSION_H
#define SESSION_H

#include <string>

using namespace std;
//typedef struct __session {
//    struct __profile *owner;
//    int id;
//    bool isopen;
//    int cmdsockfd;
//    int nsockfd;
//} session_t;

class Session {
public:
    int commandSocket;
    int notificationSocket;
};


#endif //SESSION_H
