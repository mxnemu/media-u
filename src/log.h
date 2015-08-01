#ifndef LOG_H
#define LOG_H

class Log
{
public:
    Log();

    bool logFilesFound;
    bool logCredentials;
    bool logDatabases;
    bool logTrackers;

    bool logNetworkErrors;
    bool logError;
};

extern Log gLog; // evil global, because I'm a lazy fuck

#endif // LOG_H
