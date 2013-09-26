#ifndef APIPUSHEVENTS_H
#define APIPUSHEVENTS_H

#include <QObject>
#include <qhttpconnection.h>
#include "library.h"

class ApiPushEvents : public QObject
{
    Q_OBJECT
public:
    explicit ApiPushEvents(const Library& library, QObject *parent = 0);
    
    bool handleApiRequest(QHttpRequest *req, QHttpResponse *resp);
    void sendToListeners(const QString &message, const QString &event = QString());
signals:
    
public slots:

private slots:
    void listenerTerminated();
    void onShowAdded(TvShow* show);

private:
    QList<QHttpResponse*> listeners;
    const Library& library;
};

#endif // APIPUSHEVENTS_H
