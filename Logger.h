// Logger.h
#pragma once
#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel { INFO, ERROR, DEBUG };

class Logger {
private:
    std::ofstream logfile;
    std::mutex mtx;

    Logger();
    ~Logger();

public:
    static Logger& getInstance();

    void log(LogLevel level, const std::string& message);

    void info(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);
};
