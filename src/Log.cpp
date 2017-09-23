#include <string>

#include "Log.hpp"

Log::LogLevel Log::logLevel = Log::ERROR;
const string Log::LOG_NAMES[] = {"NONE", "ERROR", "INFO", "DEBUG"};
