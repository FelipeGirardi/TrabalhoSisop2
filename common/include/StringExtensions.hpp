//
// Created by Laura Corssac on 9/5/21.
//
#pragma once

#include <list>
#include <vector>
#include <string>

namespace Common
{
    class StringExtensions
    {
    public:
        /*
        * splits string with a 1-char delimiter into a vector
        * Example:
        * input: aaa+bbb+c
        * output: [aaa, bbb, c]
        */
        std::vector<std::string> split(const std::string& s, char delim);

        /**
         * Creates another string with all letters in upper case.
         *
         * Examples:
         *   some   string  -> SOME   STRING
         *   anOTHeR sTRing -> ANOTHER STRING
         *   UPPERCASE      -> UPPERCASE
         */
        static std::string toUppercase(std::string s);
    };
}
