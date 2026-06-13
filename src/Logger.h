#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>

enum LogLevel {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR 
};

struct LogEntry {
    int tick;
    LogLevel level;
    std::string source; 
    std::string message;
    
    std::string toString() const {
        return "[Tick " + std::to_string(tick) + "] [" + source + "] " + message;
    }
};

class Logger {
private:
    std::vector<LogEntry> logs;
public:
    Logger() = default;
    ~Logger() = default;

    void addLog(int tick, LogLevel level, const std::string& source, const std::string& message);

    const std::vector<LogEntry>& getLogs() const { return logs; }

    void clear();
};

#endif