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
    };
}
