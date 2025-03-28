#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

struct Logger
{
    static void debug(std::string &message);
    static void info(std::string &message);
    static void warn(std::string &message);
    static void error(std::string &message);
};

#endif
