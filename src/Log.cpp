#include <string>

#include "Log.hpp"

Log::LogLevel Log::logLevel = Log::ERROR;
const char *Log::LOG_NAMES[] = {"NONE", "ERROR", "WARNING", "INFO", "DEBUG"};
