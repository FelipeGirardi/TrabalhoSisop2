#ifndef USERINFORMATION_HPP
#define USERINFORMATION_HPP

#include <semaphore.h>
#include <string>
#include <list>
#include <pthread.h>

#include "Session.hpp"

using namespace std;
using std::string;

namespace userInformation {

    class UserInformation {

        private:

            list<string> followers; // names of followers


            /**
             * This is the method executed by the producer thread
             * @param arg - an arg_struct object
             * @return
             */
            static void * producer(void *arg);

            /**
             * This is the method executed by the consumer thread
             * @param arg - an UserInformation object
             * @return
             */
            static void *consumer(void *arg);

        public:

            //TODO: maybe make this private and add some setters and getters
            string username;
            int numberOfSessions;
            pthread_t consumerTid;
            list<string> pendingNotifications; //IDs of notifications the user still has to receive
            sem_t freeCritialSession, hasItems;
            list<Session> sessions;

            /* Initializers */
            UserInformation();
            UserInformation(string username);
            UserInformation(string username, list<string> pendingNotifications, list<string> followers);

            ~UserInformation();

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
            void decrementNumberOfSessions();

            void addNewNotification(string notificationID);
            void getNotifications();

            void addNewFollower(string follower);
            void addNewFollowers(list<string> followers);
            string toString();

            /**
             * This method creates a new producer thread
             * for adding a notification in the user's pendingNotifications
             *
             */
            void produceNewNotification(string notificationID);
            void startListeningForNotifications();
            void stopListeningForNotifications();

    };
}

#endif