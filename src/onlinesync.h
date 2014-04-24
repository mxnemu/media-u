#ifndef ONLINESYNCTHREAD_H
#define ONLINESYNCTHREAD_H

#include <QList>
#include "onlinetracker.h"
#include "onlinetvshowdatabase.h"

class OnlineSync : public QObject
{
    Q_OBJECT
public:
    OnlineSync();

    void init(QString configFile);
    void fetchShows(QList<TvShow*> shows, Library& library);

signals:
    void trackersFinished();
    void databasesFinished();
    void allFinished();

private:
    QList<OnlineTracker*> trackers;
    QList<OnlineTvShowDatabase::Client*> databases;
};

#endif // ONLINESYNCTHREAD_H
