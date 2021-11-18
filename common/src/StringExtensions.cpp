//
// Created by Laura Corssac on 9/5/21.
//

#include "StringExtensions.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

using namespace Common;
using namespace std;

vector<string> StringExtensions::split(const string& s, char delim) {
    vector<string> result;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

std::string StringExtensions::toUppercase(string s)
{
    string uppercaseString;
    for (auto ch : s)
        uppercaseString += (char)toupper(ch);

    return uppercaseString;
}
