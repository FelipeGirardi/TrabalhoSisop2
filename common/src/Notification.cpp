//
// Created by Laura Corssac on 9/6/21.
//

#include "../include/Notification.hpp"

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
    void Notification::setPendingReaders(int pendingReaders){
        this->pendingReaders = pendingReaders;
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

}