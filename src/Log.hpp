#ifndef LOG_HPP_
#define LOG_HPP_

#include <stdexcept>
#include <cstdarg>
#include <cstdio>
#include <string>

using namespace std;

class Log {
public:
    enum LogLevel { NONE, ERROR, WARNING, INFO, DEBUG };
    static LogLevel logLevel;

private:
    static const char *LOG_NAMES[];

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
            fprintf(stream, "%-11s", LOG_NAMES[logLevel]);
            vfprintf(stream, message.c_str(), format_args);
            fprintf(stream, "\n");
            fflush(stream);
        }
    }

public:
    static void setLogLevel(int level) { logLevel = static_cast<LogLevel>(level); }

    static void error(string message, ...) {
        va_list format_args;
        va_start(format_args, message);
        log(message, ERROR, format_args);
        va_end(format_args);
		throw std::runtime_error("");
    }

    static void warn(string message, ...) {
        va_list format_args;
        va_start(format_args, message);
        log(message, WARNING, format_args);
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
