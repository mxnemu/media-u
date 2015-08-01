#include "log.h"

Log gLog;

Log::Log() :
    logFilesFound(false),
    logCredentials(true),
    logDatabases(true),
    logTrackers(true),
    logNetworkErrors(true),
    logError(true)
{
}
