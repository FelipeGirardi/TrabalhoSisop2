//
// Created by Laura Corssac on 10/31/21.
//

#ifndef ELECTIONMANAGER_HPP
#define ELECTIONMANAGER_HPP

#include <vector>
#include <string>
#define INVALID_SOCKET -1

using namespace std;

typedef struct {

    int _id;
    int sendSocket;
    int receiveSocket;
    string ip;

} ServerInfo;

class ElectionManager {

private:
    vector<ServerInfo> servers;

    //ID deste processo
    int currentServerID;

    //ID do servidor primário atual
    int currentCoordinatorID;

public:
    ElectionManager();
    void startElection();
    static void *send_election_message(void *data);
    void assumeCoordination();
    void setNewCoordinatorID(int newCoordinatorID);
    int getCoordinatorID();
    int getCurrentCoordinatorSendSocket();
    int getProcessID();
    void setSendSocket(int sendSocket, int serverID);
    void setReceiveSocket(int receiveSocket, int serverID);
    string getIPfromID(int serverID);
    bool isCurrentProcessCoordinator();
    void sendCoordinatorPacket(int sendSocket);
    bool hasValidSendSocketForServer(int serverID);
    void setCurrentServerID(int serverID);
    void setServers(vector<ServerInfo> servers);
    void setNewCoordinatorIDToItself();
    int getNumberOfServers();
    void sendExitToAllOtherServers();
    void printItself();
};

typedef struct {

    int socket;

} ReceiveThreadArguments;



#endif //ELECTIONMANAGER_HPP
