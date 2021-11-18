/**
 * Global configuration settings.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "config/GlobalExceptionHandler.hpp"
#include "config/GlobalSignalHandler.hpp"

 // const auto HANDLE_UNCAUGHT_EXCEPTIONS = ClientApp::Config::GlobalExceptionHandler::enable();
const auto HANDLE_RAISED_SIGNALS = ClientApp::Config::GlobalSignalHandler::enable();
