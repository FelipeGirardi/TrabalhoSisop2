#include <unordered_map>
#include <iostream>
#include <fstream>
#include "../include/profile_session_manager.hpp"

using namespace std;

namespace profileSessionManager {

    ProfileSessionManager::ProfileSessionManager(unordered_map <string, UserInformation> users,
                                                 unordered_map<int, string> notifications) {

        this->users = users;
        this->notifications = notifications;
    }

    unordered_map <string, UserInformation> ProfileSessionManager::getUsersFromFile() {

        unordered_map <string, UserInformation> users;

        string line;
        ifstream myfile ("example.txt");
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                cout << line << '\n';
            }
            myfile.close();
        }

        else cout << "Unable to open file";

        return users;

    }

    void ProfileSessionManager::saveUsersOnFile() {

        ofstream file;
        file.open ("example.txt");
        file << "Writing this to a file.\n";
        file.close();

    }

}