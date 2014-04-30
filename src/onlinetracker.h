#ifndef ONLINETRACKER_H
#define ONLINETRACKER_H

#include <QObject>
#include "tvshow.h"
#include "onlinecredentials.h"

class OnlineTracker : public QObject
{
    Q_OBJECT
public:
    enum UpdateResult {
        invalid,
        failedDueToMissingData,
        failedDueToNetwork,
        success,
        alreadySameAsLocal,
        skipDueToNoChanges
    };

    explicit OnlineTracker(const OnlineCredentials& credentials, QObject *parent = 0);
    virtual UpdateResult updateRemote(const TvShow* show) = 0;
    virtual bool fetchRemote(QList<TvShow*>& shows) = 0;
    virtual const QString identifierKey() const = 0;
signals:
    
public slots:   

protected:
    const OnlineCredentials& credentials;    
};

#endif // ONLINETRACKER_H
