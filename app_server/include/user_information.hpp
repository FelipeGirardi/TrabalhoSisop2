#ifndef USERINFORMATION_HPP
#define USERINFORMATION_HPP

#include <semaphore.h>
#include <string>
#include <list>
#include <pthread.h>

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

            /* Setters */
            void setFollowers(list<string> followers);
            void setNotifications(list<string> notificationsIDs);

            /* Other methods */
            void setNumberOfSessions(int numberOfSessions);
            void incrementNumberOfSessions();

            void addNewNotification(string notificationID);
            void getNotifications();

            void addNewFollower(string follower);
            void addNewFollowers(list<string> followers);
            string toString();

    };
}

#endif