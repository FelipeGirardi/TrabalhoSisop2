//
// Created by Laura Corssac on 9/6/21.
//

#include <unordered_map>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <list>
#include "../include/FileManager.hpp"
#include "../include/profile_session_manager.hpp"
#include "../include/utils/StringExtensions.hpp"
#include "../include/user_information.hpp"

using namespace userInformation;

unordered_map <string, UserInformation> FileManager::getUsersFromFile() {

    unordered_map <string, UserInformation> users;
    StringExtensions stringParser;

    string line;
    ifstream myfile (this->filename);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            UserInformation newUserInfo;
            vector<string> splitedString = stringParser.split(line, this->delimiter);

            if (splitedString.size() == 3) {

                string username = splitedString[0];
                vector<string> followers = stringParser.split(splitedString[1],
                                                              this->arrayDelimiter);
                list<string> followersList(followers.begin(), followers.end());
                vector<string> notifications = stringParser.split(splitedString[2],
                                                                  this->arrayDelimiter);
                list<string> notificationsList(notifications.begin(), notifications.end());

                newUserInfo.setNumberOfSessions(0);
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

void FileManager::saveUsersOnFile(unordered_map<string, UserInformation>users) {

    ofstream file;
    file.open (this->filename);
    if (file.is_open()) {

        for (auto user_pair: users) {
            file << user_pair.first << this->delimiter;
            UserInformation userInfo = user_pair.second;
            for (string follower : userInfo.getFollowers()) {
                file << follower << this->arrayDelimiter;
            }
            file << this->delimiter;
            for (string notification : userInfo.getPendingNotifications()) {
                file << notification << this->arrayDelimiter;
            }
            file << this->delimiter << endl;
        }
        file.close();
    } else {
        cout << "Unable to open file";
    }
}