#include "onlinetracker.h"

OnlineTracker::OnlineTracker(const OnlineCredentials& credentials, QObject *parent) :
    QObject(parent),
    credentials(credentials)
{
}
