//
// Created by Laura Corssac on 9/6/21.
//

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include "UserInformation.hpp"
#include "../../Common/include/Notification.hpp"
#include "ServerAndFrontEndInfo.h"
#include "ElectionManager.hpp"

using namespace userInformation;
using namespace notification;

class FileManager {

private:
    const char delimiter = '|';
    const char arrayDelimiter = ',';
    const string serversFilename = "servers.txt";
    const string frontEndsFilename = "frontEnds.txt";

public:

    /*
     * Retrieve the property `server` to from .txt file
     */
    vector<ServerInfo> getServersFromFile();

    vector<FrontEndInfo> getFrontEndsFromFile();

};


#endif //FILEMANAGER_HPP
