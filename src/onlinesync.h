#ifndef ONLINESYNCTHREAD_H
#define ONLINESYNCTHREAD_H

#include <QList>
#include <QWaitCondition>
#include <QThread>
#include "onlinetracker.h"
#include "onlinetvshowdatabase.h"

class BaseConfig;
class OnlineSync : public QThread
{
    Q_OBJECT
public:
    OnlineSync(const Library& library);

    void init(const BaseConfig& config);
    void addShowToFetch(TvShow* show);
    void addShowToUpdate(TvShow* show);

    void run();
signals:
    void trackersFinished();
    void databasesFinished();
    void allFinished();

private slots:
    void checkIfAllFinished();

private:
    void startThreadIfNotRunning();
    bool requiresFetch(const TvShow* show, const QString dbIdentifier);
    bool fetchShow(TvShow* show, const Library& library);
    bool updateShow(TvShow* show);
    void updateTrackers();
    void fetchDatabases();


    bool shouldQuit;
    std::set<TvShow*> unhandledFetch;
    std::set<TvShow*> unhandledUpdate;
    const Library& library;
    QWaitCondition waitCondition;

    QList<OnlineCredentials*> credentials;
    QList<OnlineTracker*> trackers;
    QList<OnlineTvShowDatabase::Client*> databases;
};

#endif // ONLINESYNCTHREAD_H
