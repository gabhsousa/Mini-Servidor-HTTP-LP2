// Logger.cpp
#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>

Logger::Logger() {
    logfile.open("server.log", std::ios::app);
}

Logger::~Logger() {
    logfile.close();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::string levelStr;
    switch (level) {
        case LogLevel::INFO:  levelStr = "INFO"; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
    }

    logfile << "[" << buf << "] [" << levelStr << "] " << message << std::endl;
    logfile.flush();

    std::cout << "[" << buf << "] [" << levelStr << "] " << message << std::endl;
}

void Logger::info(const std::string& message) { log(LogLevel::INFO, message); }
void Logger::error(const std::string& message) { log(LogLevel::ERROR, message); }
void Logger::debug(const std::string& message) { log(LogLevel::DEBUG, message); }
