/**
 * Unit tests for ConcurrentCommandLine class.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "io/ConcurrentCommandLine.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <string>

using namespace ClientApp::IO;

using std::thread;
using std::packaged_task;
using std::future;
using std::string;
using std::move;
using std::cout;

namespace ClientApp
{
    namespace Tests
    {
        void initialize_MultipleThreads_InitializesOnce();
        void terminate_MultipleThreads_TerminatesOnce();
        void terminate_NotInitialized_DoesNothing();
        void readLine_NotInitialized_ReturnsBlankString();
        void readLine_MaxCharsIsNotPositive_ReturnsBlankString();
        void readLine_MaxCharsIsPositive_ReadsStringUpToMaxChars();
        void readLine_MultipleThreads_ReadsStringsInSequence();
        void writeLine_ReadLineInParallel_RunsOperationsInSequence();
        void writeLine_AnyString_WritesToConsole();
        void writeLine_MultipleThreads_WritesStringsInSequence();
    }
}

int main()
{
    void (*testFunctions[])() = {
        ClientApp::Tests::initialize_MultipleThreads_InitializesOnce,
        ClientApp::Tests::terminate_MultipleThreads_TerminatesOnce,
        ClientApp::Tests::terminate_NotInitialized_DoesNothing,
        ClientApp::Tests::readLine_NotInitialized_ReturnsBlankString,
        ClientApp::Tests::readLine_MaxCharsIsNotPositive_ReturnsBlankString,
        // ClientApp::Tests::readLine_MaxCharsIsPositive_ReadsStringUpToMaxChars,
        // ClientApp::Tests::readLine_MultipleThreads_ReadsStringsInSequence,
        // ClientApp::Tests::writeLine_ReadLineInParallel_RunsOperationsInSequence,
        ClientApp::Tests::writeLine_AnyString_WritesToConsole,
        ClientApp::Tests::writeLine_MultipleThreads_WritesStringsInSequence
    };

    for (auto testFunction : testFunctions)
    {
        testFunction();
    }
}


// Implementations

void ClientApp::Tests::initialize_MultipleThreads_InitializesOnce()
{
    packaged_task<bool()> task1(ConcurrentCommandLine::initialize);
    packaged_task<bool()> task2(ConcurrentCommandLine::initialize);
    packaged_task<bool()> task3(ConcurrentCommandLine::initialize);

    auto result1 = task1.get_future();
    auto result2 = task2.get_future();
    auto result3 = task3.get_future();

    auto thread1 = thread(move(task1));
    auto thread2 = thread(move(task2));
    auto thread3 = thread(move(task3));

    thread1.join();
    thread2.join();
    thread3.join();

    ConcurrentCommandLine::terminate();
    cout << "initialize_MultipleThreads_InitializesOnce\r\n"
        << "    thread1 = " << result1.get() << "\r\n"
        << "    thread2 = " << result2.get() << "\r\n"
        << "    thread3 = " << result3.get() << "\r\n\r\n";
}

void ClientApp::Tests::terminate_MultipleThreads_TerminatesOnce()
{
    ConcurrentCommandLine::initialize();

    packaged_task<bool()> task1(ConcurrentCommandLine::terminate);
    packaged_task<bool()> task2(ConcurrentCommandLine::terminate);
    packaged_task<bool()> task3(ConcurrentCommandLine::terminate);

    auto result1 = task1.get_future();
    auto result2 = task2.get_future();
    auto result3 = task3.get_future();

    auto thread1 = thread(move(task1));
    auto thread2 = thread(move(task2));
    auto thread3 = thread(move(task3));

    thread1.join();
    thread2.join();
    thread3.join();

    cout << "terminate_MultipleThreads_TerminatesOnce\r\n"
        << "    thread1 = " << result1.get() << "\r\n"
        << "    thread2 = " << result2.get() << "\r\n"
        << "    thread3 = " << result3.get() << "\r\n\r\n";
}

void ClientApp::Tests::terminate_NotInitialized_DoesNothing()
{
    auto hasChanges = ConcurrentCommandLine::terminate();

    cout << "terminate_NotInitialized_DoesNothing\r\n"
        << "    " << (hasChanges ? "failed" : "passed") << "\r\n\r\n";
}

void ClientApp::Tests::readLine_NotInitialized_ReturnsBlankString()
{
    auto input = ConcurrentCommandLine::readLine(10);

    cout << "readLine_NotInitialized_ReturnsBlankString\r\n"
        << "    " << (input.empty() ? "passed" : "failed") << "\r\n\r\n";
}

void ClientApp::Tests::readLine_MaxCharsIsNotPositive_ReturnsBlankString()
{
    ConcurrentCommandLine::initialize();

    auto input1 = ConcurrentCommandLine::readLine(0);
    auto input2 = ConcurrentCommandLine::readLine(-1);

    ConcurrentCommandLine::terminate();
    cout << "readLine_MaxCharsIsNotPositive_ReturnsBlankString\r\n"
        << "    maxChars = 0 -> " << (input1.empty() ? "passed" : "failed") << "\r\n"
        << "    maxChars < 0 -> " << (input2.empty() ? "passed" : "failed") << "\r\n\r\n";
}

void ClientApp::Tests::readLine_MaxCharsIsPositive_ReadsStringUpToMaxChars()
{
    ConcurrentCommandLine::initialize();

    auto input = ConcurrentCommandLine::readLine(10);

    ConcurrentCommandLine::terminate();
    cout << "readLine_MaxCharsIsPositive_ReadsStringUpToMaxChars\r\n"
        << "    " << input << "\r\n\r\n";
}

void ClientApp::Tests::readLine_MultipleThreads_ReadsStringsInSequence()
{
    ConcurrentCommandLine::initialize();

    packaged_task<string(int)> task1(ConcurrentCommandLine::readLine);
    packaged_task<string(int)> task2(ConcurrentCommandLine::readLine);
    packaged_task<string(int)> task3(ConcurrentCommandLine::readLine);

    auto result1 = task1.get_future();
    auto result2 = task2.get_future();
    auto result3 = task3.get_future();

    auto thread1 = thread(move(task1), 10);
    auto thread2 = thread(move(task2), 10);
    auto thread3 = thread(move(task3), 10);

    thread1.join();
    thread2.join();
    thread3.join();

    ConcurrentCommandLine::terminate();
    cout << "readLine_MultipleThreads_ReadsStringsInSequence\r\n"
        << "    thread1 = " << result1.get() << "\r\n"
        << "    thread2 = " << result2.get() << "\r\n"
        << "    thread3 = " << result3.get() << "\r\n\r\n";
}

void ClientApp::Tests::writeLine_AnyString_WritesToConsole()
{
    ConcurrentCommandLine::initialize();

    ConcurrentCommandLine::writeLine("Testing 'writeLine_AnyString_WritesToConsole'...");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    ConcurrentCommandLine::terminate();
}

void ClientApp::Tests::writeLine_MultipleThreads_WritesStringsInSequence()
{
    ConcurrentCommandLine::initialize();

    auto thread1 = thread(ConcurrentCommandLine::writeLine, "Testing 'writeLine_MultipleThreads_WritesStringsInSequence' - thread 1...");
    auto thread2 = thread(ConcurrentCommandLine::writeLine, "Testing 'writeLine_MultipleThreads_WritesStringsInSequence' - thread 2...");
    auto thread3 = thread(ConcurrentCommandLine::writeLine, "Testing 'writeLine_MultipleThreads_WritesStringsInSequence' - thread 3...");

    thread1.join();
    thread2.join();
    thread3.join();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    ConcurrentCommandLine::terminate();
}

void ClientApp::Tests::writeLine_ReadLineInParallel_RunsOperationsInSequence()
{
    ConcurrentCommandLine::initialize();

    auto thread1 = thread(ConcurrentCommandLine::writeLine, "Testing 'writeLine_ReadLineInParallel_RunsOperationsInSequence' - thread 1...");
    auto thread2 = thread(ConcurrentCommandLine::readLine, 10);
    auto thread3 = thread(ConcurrentCommandLine::writeLine, "Testing 'writeLine_ReadLineInParallel_RunsOperationsInSequence' - thread 3...");
    auto thread4 = thread(ConcurrentCommandLine::writeLine, "Testing 'writeLine_ReadLineInParallel_RunsOperationsInSequence' - thread 4...");

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    ConcurrentCommandLine::terminate();
}
