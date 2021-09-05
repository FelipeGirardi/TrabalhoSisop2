#ifndef USERINFORMATION_HPP
#define USERINFORMATION_HPP

#include <string>
#include <list>

using namespace std;
using std::string;

namespace userInformation {

    class UserInformation {

        private:
            list<int> pendingNotifications; //IDs of notifications the user still has to receive
            list<string> followers; // names of followers
        
        public:
            UserInformation();

            UserInformation(list<int> pendingNotifications, list<string> followers);
            list<string> getFollowersID;
            list<int> getPendingNotifications;

            void addNewFollower(string follower);
            void addNewFollowers(list<string> followers);
            void setFollowers(list<string> followers);

            void addNewNotification(int notificationID);
            void addNewNotifications(list<int> notificationsIDs);
            void setNotifications(list<int> notificationsIDs);
    };
}

#endif