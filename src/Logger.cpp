#include "Logger.h"

void Logger::addLog(int tick, LogLevel level, const std::string& source, const std::string& message) {
    LogEntry entry;
    entry.tick = tick;
    entry.level = level;
    entry.source = source;
    entry.message = message;
    
    logs.push_back(entry);
    //[DEBUG]
    std::printf("%s\n", entry.toString().c_str());
}

void Logger::clear() {
    logs.clear();
}