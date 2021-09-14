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
#include "../include/profile_session_manager.hpp"
#include "../include/utils/StringExtensions.hpp"

using namespace userInformation;
using namespace notification;
using namespace std;

unordered_map <string, UserInformation> FileManager::getUsersFromFile() {

    unordered_map <string, UserInformation> users;
    StringExtensions stringParser;

    string line;
    ifstream myfile (this->filename);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            //UserInformation newUserInfo;
            vector<string> splitedString = stringParser.split(line, this->delimiter);

            if (splitedString.size() == 3) {

                string username = splitedString[0];
                vector<string> followers = stringParser.split(splitedString[1],
                                                              this->arrayDelimiter);
                list<string> followersList(followers.begin(), followers.end());
                vector<string> notifications = stringParser.split(splitedString[2],
                                                                  this->arrayDelimiter);
                list<string> notificationsList(notifications.begin(), notifications.end());

//                newUserInfo.username = username;
//                newUserInfo.setNumberOfSessions(0);
//                newUserInfo.setFollowers(followersList);
//                newUserInfo.setNotifications(notificationsList);

                users[username] = UserInformation(username, notificationsList, followersList);
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
void FileManager::saveNotificationsOnFile(unordered_map<string, Notification> notifications) {
    ofstream file;
    file.open (this->notificationsFilename);
    if (file.is_open()) {
        for (auto notif_pair: notifications) {
            file << notif_pair.first << this->delimiter;
            Notification notif = notif_pair.second;
            file << notif.toString();
            file << this->delimiter << endl;
        }
        file.close();
    } else {
        cout << "Unable to open file";
    }

}
unordered_map<string, Notification> FileManager::getNotificationsFromFile() {
    unordered_map <string, Notification> notifications;
    StringExtensions stringParser;

    string line;
    ifstream myfile (this->notificationsFilename);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            vector<string> splitedString = stringParser.split(line, this->delimiter);

            //TODO: dar um jeito nesse 5
            if (splitedString.size() == 5) {
                string id = splitedString[0];
                string text = splitedString[1];
                string username = splitedString[2];
                long int time = stoi(splitedString[3]);
                int pendingReaders = stoi(splitedString[4]);

                Notification newNotif = Notification(id, text, username,
                                                     time, pendingReaders);
                notifications[id] = newNotif;
            } else {
                cout << "Invalid file line";
            }
        }
        myfile.close();
    }
    else cout << "Unable to open file";
    return notifications;

}
