//
// Created by Laura Corssac on 9/11/21.
//

#include "../../include/utils/ListExtensions.hpp"
#include <iostream>

using namespace std;

void ListExtensions::printList(list<string> list)
{
    cout << "Printing List" << endl;
    for (string i : list) {
        std::cout << i << " ";
    }
    cout << "End Printing List" << endl;
}
