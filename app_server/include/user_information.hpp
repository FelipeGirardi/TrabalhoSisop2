#ifndef USERINFORMATION_HPP
#define USERINFORMATION_HPP

#include <string>
#include <list>

using namespace std;
using std::string;

namespace userInformation {

    class UserInformation {

        private:
            list<string> pendingNotifications; //IDs of notifications the user still has to receive
            list<string> followers; // names of followers
            int numerOfSessions;
        
        public:
            /* Initializers */
            UserInformation();
            UserInformation(list<string> pendingNotifications, list<string> followers);

            /* Getters */
            list<string> getFollowers();
            list<string> getPendingNotifications();
            int getNumberOfSessions();

            /* Setters Followers */
            void addNewFollower(string follower);
            void addNewFollowers(list<string> followers);
            void setFollowers(list<string> followers);

            /* Setters Notifications */
            void addNewNotification(string notificationID);
            void addNewNotifications(list<string> notificationsIDs);
            void setNotifications(list<string> notificationsIDs);

            /* Other Setters */
            void setNumberOfSessions(int numberOfSessions);
            void incrementNumberOfSessions();

            /* Other methods */
            string toString();

    };
}

#endif