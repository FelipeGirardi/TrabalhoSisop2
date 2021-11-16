//
// Created by Laura Corssac on 9/6/21.
//

#include <unordered_map>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <list>
#include <string>
#include "../include/FileManager.hpp"
#include "../include/ProfileSessionManager.hpp"
#include "../include/utils/StringExtensions.hpp"

using namespace userInformation;
using namespace notification;
using namespace std;

vector<FrontEndInfo> FileManager::getFrontEndsFromFile() {
    StringExtensions stringParser;
    vector<FrontEndInfo> frontEndsInfo;

    string line;
    ifstream myfile (this->frontEndsFilename);
    if (myfile.is_open()) {

        // line format = front end IP | front end port |
        while ( getline (myfile,line) )
        {
            vector<string> splitedString = stringParser.split(line, this->delimiter);
            if (splitedString.size() < 2) {
                cout << "Invalid file line";
                break;
            }

            string ip = splitedString[0];
            int port = stoi(splitedString[1]);

            FrontEndInfo newFrontEndInfo = {
                    .ip=ip,
                    .port=port,
                    .sendSocket = INVALID_SOCKET,
                    .receiveSocket = INVALID_SOCKET
            };
            frontEndsInfo.push_back(newFrontEndInfo);
        }

        myfile.close();
    }
    else cout << "Unable to open front ends file";
    return frontEndsInfo;
}

vector<ServerInfo> FileManager::getServersFromFile() {
    StringExtensions stringParser;
    vector<ServerInfo> serversInfo;

    string line;
    ifstream myfile (this->serversFilename);
    if (myfile.is_open()) {

        // line format = server ID | server IP |
        while ( getline (myfile,line) )
        {
            vector<string> splitedString = stringParser.split(line, this->delimiter);
            if (splitedString.size() < 2) {
                cout << "Invalid file line";
                break;
            }
            int serverID = stoi(splitedString[0]);
            string serverIP = splitedString[1];

            ServerInfo newServerInfo = {
                                        ._id=serverID,
                                        .sendSocket=INVALID_SOCKET,
                                        .receiveSocket=INVALID_SOCKET,
                                        .ip=serverIP
            };
            serversInfo.push_back(newServerInfo);
        }

        myfile.close();
    }
    else cout << "Unable to open servers file";
    return serversInfo;
}
