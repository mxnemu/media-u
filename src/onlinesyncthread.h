#ifndef ONLINESYNCTHREAD_H
#define ONLINESYNCTHREAD_H

#include <QList>
#include "onlinetracker.h"
#include "onlinetvshowdatabase.h"

class OnlineSyncThread
{
public:
    OnlineSyncThread();
    search

private:
    QList<OnlineTracker> trackers;
    QList<OnlineTvShowDatabase> databases;
};

#endif // ONLINESYNCTHREAD_H
