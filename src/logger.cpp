#include "logger.h"

#include <stdexcept>
#include <ctime>
#include <QDebug>

using namespace std;

const string Logger::kLogLevelDebug = "DEBUG";
const string Logger::kLogLevelInfo = "INFO";
const string Logger::kLogLevelError = "ERROR";

const char* const Logger::kLogFileName = "/tmp/cgtn.log";

Logger* Logger::pInstance = nullptr;

mutex Logger::sMutex;

Logger& Logger::instance()
{
    static Cleanup cleanup;

    lock_guard<mutex> guard(sMutex);
    if(pInstance == nullptr)
    {
        pInstance = new Logger();
    }

    return *pInstance;
}

Logger::Cleanup::~Cleanup()
{
    lock_guard<mutex> guard(Logger::sMutex);
    delete Logger::pInstance;
    Logger::pInstance = nullptr;
}

Logger::~Logger()
{
    mOutputStream.close();
}

Logger::Logger()
{
    mOutputStream.open(kLogFileName, ios_base::app);
    if(!mOutputStream.good())
    {
        throw runtime_error("Unable to initialize the Logger!");
    }
}

void Logger::log(const string inMessage)
{
    log(inMessage, kLogLevelInfo);
}

void Logger::log(const string &inMessage, const string &inLogLevel)
{
    lock_guard<mutex> guard(sMutex);
    logHelper(inMessage, inLogLevel);
}

void Logger::log(const std::vector<string> &inMessages, const string &inLogLevel)
{
    lock_guard<mutex> guard(sMutex);
    for(size_t i=0; i<inMessages.size(); i++)
    {
        logHelper(inMessages.at(i), inLogLevel);
    }
}

void Logger::logHelper(const string &inMessage, const string &inLogLevel)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
    string str(buffer);
#ifdef QT_DEBUG
    qDebug()<<QString::fromStdString(str + " [" + inLogLevel + "] " + inMessage)<<endl;
#endif
    mOutputStream<<str<<" ["<<inLogLevel<<"] "<<inMessage<<endl;
}
