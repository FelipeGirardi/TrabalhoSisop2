//
// Created by Laura Corssac on 9/6/21.
//

#ifndef NOTIFICATION_HPP
#define NOTIFICATION_HPP
#include <string>

using namespace std;
namespace notification {
    class Notification {

    private:
        string id;
        string text;
        string username;
        long int time;
        int pendingReaders;
    public:
        /* Initializers */
        Notification();
        Notification(string id, string text, string username, long int time, int pendingReaders);

        /* Getters */
        string getID();
        string getText();
        string getUsername();
        int getPendingReaders();
        long int getTime();

        /* Setters */
        void setID(string id);
        void setText(string text);
        void setUsername(string username);
        void setTime(long int time);
        void setPendingReaders(int pendingReaders);

        /* Others */
        /*
         * returns a string with all attributes,
         * in the order of declaration,
         * and comma separated
         * Example: 021939,oii,@laura,20200903,2,
         * Obs.: LET THE COMMA AT THE END
         */
        string toString();

    };
}

#endif //NOTIFICATION_HPP
