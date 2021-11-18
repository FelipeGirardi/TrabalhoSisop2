//
// Created by Laura Corssac on 9/6/21.
//

#include "Notification.hpp"
#include "StringExtensions.hpp"
#include <unordered_map>
#include <vector>
#include <list>
#include <string>

using namespace std;

namespace notification {

    Notification::Notification() { }
    Notification::Notification(string id, string text, string username,
        long int time,
        int pendingReaders) {
        this->id = id;
        this->text = text;
        this->username = username;
        this->time = time;
        this->pendingReaders = pendingReaders;
    }

    Notification* Notification::parseBytes(const char* bytes)
    {
        if (bytes != nullptr)
        {
            auto parsedNotification = (Notification*)bytes;
            return new Notification(*parsedNotification);
        }

        return nullptr;
    }

    Notification Notification::parseCsvString(const char* csvString)
    {
        return notificationFromString(string(csvString));
    }

    Notification Notification::notificationFromString(string sourceString) {

        Common::StringExtensions stringParser;
        vector<string> splitedString = stringParser.split(sourceString, ',');

        //TODO: dar um jeito nesse 5
        if (splitedString.size() == 5) {
            string id = splitedString[0];
            string text = splitedString[1];
            string username = splitedString[2];
            long int time = stoi(splitedString[3]);
            int pendingReaders = stoi(splitedString[4]);

            Notification newNotif = Notification(id, text, username,
                time, pendingReaders);
            return newNotif;
        }
        else {
            Notification emptyNot;
            return emptyNot;
        }

    }

    string Notification::getID() {
        return this->id;
    }
    string Notification::getText() {
        return this->text;
    }
    string Notification::getUsername() {
        return this->username;
    }
    int Notification::getPendingReaders() {
        return this->pendingReaders;
    }
    long int Notification::getTime() {
        return this->time;
    }
    void Notification::setID(string id) {
        this->id = id;
    }
    void Notification::setText(string text) {
        this->text = text;
    }
    void Notification::setUsername(string username) {
        this->username = username;
    }
    void Notification::setTime(long int time) {
        this->time = time;
    }
    void Notification::setPendingReaders(int pendingReaders) {
        this->pendingReaders = pendingReaders;
    }
    void Notification::decrementPendingReaders() {
        this->pendingReaders -= 1;
    }

    string Notification::toString() {
        string return_string;
        return_string += this->id + ',';
        return_string += this->text + ',';
        return_string += this->username + ',';
        return_string += to_string(this->time) + ',';
        return_string += to_string(this->pendingReaders) + ',';
        return return_string;
    }

    char* Notification::toBytes()
    {
        auto notificationCopy = new Notification(*this);
        return (char*)notificationCopy;
    }
}
