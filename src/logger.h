#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>

class Logger
{
public:
    static const std::string kLogLevelDebug;
    static const std::string kLogLevelInfo;
    static const std::string kLogLevelError;

    static Logger& instance();

    void log(const std::string inMessage);
    void log(const std::string& inMessage, const std::string& inLogLevel);

    void log(const std::vector<std::string>& inMessages, const std::string& inLogLevel);

protected:
    static Logger* pInstance;
    static const char* kLogFileName;

    std::ofstream mOutputStream;

    friend class Cleanup;
    class Cleanup
    {
    public:
        Cleanup() {}
        ~Cleanup();
    };

    void logHelper(const std::string& inMessage, const std::string& inLogLevel);

private:
    Logger();
    virtual ~Logger();
    Logger(const Logger&);
    Logger& operator =(const Logger&);
    static std::mutex sMutex;
};

#endif // LOGGER_H
