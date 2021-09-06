#ifndef USERINFORMATION_HPP
#define USERINFORMATION_HPP

//#include <semaphore.h>
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
//            sem_t hasItemsToConsume;
//            sem_t mutexP, mutexC;

        
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
            void addNewNotifications(list<string> notificationsIDs);

            void addNewFollower(string follower);
            void addNewFollowers(list<string> followers);
            string toString();

    };
}

#endif