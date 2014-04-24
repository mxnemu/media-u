#ifndef ONLINETRACKER_H
#define ONLINETRACKER_H

#include <QObject>
#include "tvshow.h"
#include "onlinecredentials.h"

class OnlineTracker : public QObject
{
    Q_OBJECT
public:
    explicit OnlineTracker(const OnlineCredentials& credentials, QObject *parent = 0);
    virtual bool updateRemote(TvShow* show) = 0;
    virtual bool fetchRemote(QList<TvShow*>& shows) = 0;
signals:
    
public slots:

protected:
    const OnlineCredentials& credentials;
    virtual const QString identifierKey() = 0;
    
};

#endif // ONLINETRACKER_H
