//
// Created by Laura Corssac on 9/5/21.
//

#ifndef STRINGEXTENSIONS_HPP
#define STRINGEXTENSIONS_HPP

#include <list>
#include <vector>
#include <string>

using namespace std;

class StringExtensions {

    public:
        /* Initializes */
        StringExtensions();

        /*
        * splits string with a 1-char delimiter into a vector
        * Example:
        * input: aaa+bbb+c
        * output: [aaa, bbb, c]
        */
        vector<string> split(const string &s, char delim);

};


#endif //STRINGEXTENSIONS_HPP
