/**
 * Thread-safe wrapper for command line operations.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include <curses.h>
#include "io/ConcurrentCommandLine.hpp"

using namespace ClientApp::IO;

using std::mutex;
using std::string;

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
        showInput();

        char* rawInput = new char[maxChars + 1];
        getnstr(rawInput, maxChars);
        rawInput[maxChars] = '\0';

        input = string(rawInput);

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
    nodelay(stdscr, FALSE);
    echo();
}

void ConcurrentCommandLine::hideInput()
{
    nodelay(stdscr, TRUE);
    noecho();
}
