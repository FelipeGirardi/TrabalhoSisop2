//
// Created by Laura Corssac on 9/6/21.
//

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <unordered_map>
#include <string>
#include "UserInformation.hpp"
#include "../../Common/include/Notification.hpp"

using namespace userInformation;
using namespace notification;

class FileManager {

private:
    const char delimiter = '|';
    const char arrayDelimiter = ',';
    const string filename = "users.txt";
    const string notificationsFilename = "notifications.txt";

public:
    /*
     * Save the SessionManager's property `users` to a .txt file
     * Each line:
     * username | follower1,follower2, | notifID1, notifID2, |
     */
    void saveUsersOnFile(unordered_map<string, UserInformation>);

    /*
     * Retrieve the property `users` to from .txt file
     */
    unordered_map<string, UserInformation> getUsersFromFile();

    /*
     * Save the SessionManager's property `notifications` to a .txt file
     * Each line:
     * notification ID | attributes of the notification (see notification.toString()) |
     * Obs.: LET THE DELIMITER AT THE END
     */
    void saveNotificationsOnFile(unordered_map<string, Notification> notifications);

    /*
     * Retrieve the property `notifications` to from .txt file
     */
    unordered_map<string, Notification> getNotificationsFromFile();

};


#endif //FILEMANAGER_HPP
