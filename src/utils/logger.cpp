#include <ctime>
#include <iostream>
#include <sstream>

#include "utils/logger.hpp"

void Logger::info(std::string &message)
{
    char buffer[80];
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", now);

    std::stringstream ss;
    ss << "[INFO] " << "[" << buffer << "] " << message;
    std::string output = ss.str();

    std::cout << output << std::endl;
}

void Logger::debug(std::string &message)
{
    char buffer[80];
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", now);

    std::stringstream ss;
    ss << "[DEBUG] " << "[" << buffer << "] " << message;
    std::string output = ss.str();

    std::cout << output << std::endl;
}

void Logger::warn(std::string &message)
{
    char buffer[80];
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", now);

    std::stringstream ss;
    ss << "[WARN] " << "[" << buffer << "] " << message;
    std::string output = ss.str();

    std::cout << output << std::endl;
}

void Logger::error(std::string &message)
{
    char buffer[80];
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", now);

    std::stringstream ss;
    ss << "[ERROR] " << "[" << buffer << "] " << message;
    std::string output = ss.str();

    std::cout << output << std::endl;
}
