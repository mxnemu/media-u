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
    void fetchShow(TvShow* show, Library& library);
    void updateShow(TvShow* show);

signals:
    void trackersFinished();
    void databasesFinished();
    void allFinished();

private slots:
    void checkIfAllFinished();

private:
    bool requiresFetch(const TvShow* show, const QString dbIdentifier);

    std::set<TvShow*> unhandledFetch;
    std::set<TvShow*> unhandledUpdate;

    QList<OnlineCredentials*> credentials;
    QList<OnlineTracker*> trackers;
    QList<OnlineTvShowDatabase::Client*> databases;
};

#endif // ONLINESYNCTHREAD_H
