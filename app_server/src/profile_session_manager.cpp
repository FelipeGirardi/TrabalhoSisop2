#include <unordered_map>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <list>
#include "../include/profile_session_manager.hpp"
#include "../include/utils/StringExtensions.hpp"

using namespace std;

namespace profileSessionManager {

    ProfileSessionManager::ProfileSessionManager(unordered_map <string, UserInformation> users,
                                                 unordered_map<string, string> notifications) {

        this->users = users;
        this->notifications = notifications;
    }

    unordered_map <string, UserInformation> ProfileSessionManager::getUsersFromFile() {

        unordered_map <string, UserInformation> users;
        StringExtensions stringParser;

        string line;
        ifstream myfile ("example.txt");
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                UserInformation newUserInfo;
                vector<string> splitedString = stringParser.split(line, '|');

                if (splitedString.size() == 3) {

                    string username = splitedString[0];
                    vector<string> followers = stringParser.split(splitedString[1], ',');
                    list<string> followersList(followers.begin(), followers.end());
                    vector<string> notifications = stringParser.split(splitedString[2], ',');
                    list<string> notificationsList(notifications.begin(), notifications.end());

                    newUserInfo.setFollowers(followersList);
                    newUserInfo.setNotifications(notificationsList);
                    users[username] = newUserInfo;
                } else {
                    cout << "Invalid file line";
                }
            }
            myfile.close();
        }

        else cout << "Unable to open file";

        return users;

    }

    void ProfileSessionManager::saveUsersOnFile() {

        ofstream file;
        file.open ("example.txt");
        if (file.is_open()) {

            for (auto user_pair: this->users) {
                file << user_pair.first << '|';
                UserInformation userInfo = user_pair.second;
                for (string follower : userInfo.getFollowers()) {
                    file << follower << ",";
                }
                file << '|';
                for (string notification : userInfo.getPendingNotifications()) {
                    file << notification << ",";
                }
                file << '|' << endl;
            }
            file.close();
        } else {
            cout << "Unable to open file";
        }
    }

    void ProfileSessionManager::setUsers(unordered_map<string,UserInformation> users) {
        this->users = users;
    }

    unordered_map<string,UserInformation> ProfileSessionManager::getUsers() {
        return this->users;
    }

    unordered_map<string,string> ProfileSessionManager::getNotifications() {
        return this->notifications;
    }

}