//
// Created by Laura Corssac on 10/31/21.
//

#ifndef ELECTIONMANAGER_HPP
#define ELECTIONMANAGER_HPP

#include <vector>

using namespace std;

typedef struct {

    int _id;
    int sendSocket;
    int receiveSocket;

} ServerInfo;

class ElectionManager {

private:
    vector<ServerInfo> servers;
    int currentServerID;

public:
    ElectionManager();
    void startElection();
    static void *receive_messages(void *data);
    static void *send_election_message(void *data);

};

typedef struct {

    int socket;

} ReceiveThreadArguments;



#endif //ELECTIONMANAGER_HPP
