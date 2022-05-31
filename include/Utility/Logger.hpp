#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

enum class LogLevel : unsigned int {
    Debug,
    Info,
    Warning,
    Error,
    SDLError,
};

struct Logger {
    static void ShowMe();
    static void Message(LogLevel log_level, const std::string& message);
    static void ShowGLInfo();

    static void Spacing();
    static void Separator();

    static std::string GetTimestamp();
};

#endif