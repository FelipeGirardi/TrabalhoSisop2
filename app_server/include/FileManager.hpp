//
// Created by Laura Corssac on 9/6/21.
//

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <unordered_map>
#include <string>
#include "../include/user_information.hpp"

using namespace userInformation;

class FileManager {

    private:
        const char delimiter = '|';
        const char arrayDelimiter = ',';
        const string filename = "users.txt";

    public:
        /*
         * Save the property `users` to a .txt file
         */
        void saveUsersOnFile(unordered_map<string, UserInformation>);
        /*
         * Retrieve the property `users` to from .txt file
         */
        unordered_map<string, UserInformation> getUsersFromFile();

};


#endif //FILEMANAGER_HPP
