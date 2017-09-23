#ifndef LOG_HPP_
#define LOG_HPP_

#include <cstdarg>
#include <cstdio>
#include <string>

using namespace std;

class Log {
public:
    enum LogLevel { NONE, ERROR, INFO, DEBUG };
    static LogLevel logLevel;

private:
    static const string LOG_NAMES[];

    static FILE *getOutStream(LogLevel logLevel) {
        switch (logLevel) {
        case ERROR:
            return stderr;
        default:
            return stdout;
        }
    }

    static void log(string message, LogLevel logLevel, va_list format_args) {
        if (Log::logLevel >= logLevel) {
            FILE *stream = getOutStream(logLevel);
            fprintf(stream, "%s:\t", LOG_NAMES[logLevel].c_str());
            vfprintf(stream, message.c_str(), format_args);
            fprintf(stream, "\n");
        }
    }

public:
    static void setLogLevel(int level) { logLevel = static_cast<LogLevel>(level); }

    static void error(string message, ...) {
        va_list format_args;
        va_start(format_args, message);
        log(message, ERROR, format_args);
        va_end(format_args);
    }

    static void info(string message, ...) {
        va_list format_args;
        va_start(format_args, message);
        log(message, INFO, format_args);
        va_end(format_args);
    }

    static void debug(string message, ...) {
        va_list format_args;
        va_start(format_args, message);
        log(message, DEBUG, format_args);
        va_end(format_args);
    }
};

#endif
