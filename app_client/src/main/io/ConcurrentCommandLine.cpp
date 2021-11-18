/**
 * Thread-safe wrapper for command line operations.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include <curses.h>
#include <iostream>
#include <limits>
#include <ios>
#include <csignal>
#include <cstdlib>
#include "config/GlobalConstants.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include "io/CommandLineParser.hpp"
#include "StringExtensions.hpp"

using namespace ClientApp::IO;
using namespace std;

using Common::StringExtensions;

// Static variables

mutex ConcurrentCommandLine::cliMutex_;
bool ConcurrentCommandLine::isInitialized_ = false;

// Public methods

bool ConcurrentCommandLine::initialize()
{
    cliMutex_.lock();

    bool hasChanges = false;
    if (!isInitialized())
    {
        initscr();
        cbreak();
        curs_set(0);
        scrollok(stdscr, TRUE);

        hideInput();

        setInitialized(true);
        hasChanges = true;
    }

    cliMutex_.unlock();
    return hasChanges;
}

bool ConcurrentCommandLine::terminate()
{
    cliMutex_.lock();

    bool hasChanges = false;
    if (isInitialized())
    {
        endwin();

        setInitialized(false);
        hasChanges = true;
    }

    cliMutex_.unlock();
    return hasChanges;
}

string ConcurrentCommandLine::readLine(int maxChars)
{
    cliMutex_.lock();

    string input;
    if (isInitialized() && maxChars > 0)
    {
        printw("You: ");
        refresh();
        showInput();

        char* rawInput = new char[maxChars + 1];
        auto numCharsRead = getnstr(rawInput, maxChars);
        rawInput[maxChars] = '\0';

        input = string(rawInput);

        printw("\n");
        refresh();
        hideInput();
    }

    cliMutex_.unlock();
    return input;
}

void ConcurrentCommandLine::writeLine(string output)
{
    cliMutex_.lock();

    if (isInitialized())
    {
        printw("%s\n", output.c_str());
        refresh();
    }

    cliMutex_.unlock();
}

void ConcurrentCommandLine::waitInput()
{
    int enteredKey = getch();
    if (enteredKey == AsciiCode::END_OF_TRANSMISSION)
        raise(SIGINT);

    cliMutex_.lock();
    ungetch(enteredKey);
    cliMutex_.unlock();
}

// Private methods

bool ConcurrentCommandLine::isInitialized()
{
    return isInitialized_;
}

void ConcurrentCommandLine::setInitialized(bool isInitialized)
{
    ConcurrentCommandLine::isInitialized_ = isInitialized;
}

void ConcurrentCommandLine::showInput()
{
    echo();
}

void ConcurrentCommandLine::hideInput()
{
    noecho();
}
