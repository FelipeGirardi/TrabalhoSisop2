//
// Created by Laura Corssac on 9/5/21.
//

#include "../../include/utils/StringExtensions.hpp"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

StringExtensions::StringExtensions() {

}

vector<string> StringExtensions::split(const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}