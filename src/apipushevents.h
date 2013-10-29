#ifndef APIPUSHEVENTS_H
#define APIPUSHEVENTS_H

#include <QObject>
#include <qhttpconnection.h>
#include "library.h"
#include <videoplayer.h>

class ApiPushEvents : public QObject
{
    Q_OBJECT
public:
    explicit ApiPushEvents(const Library& library, const VideoPlayer& videoplayer, QObject *parent = 0);
    
    bool handleApiRequest(QHttpRequest *req, QHttpResponse *resp);
    void sendToListeners(const QString &message, const QString &event = QString());
signals:
    
public slots:

private slots:
    void listenerTerminated();
    void onShowAdded(TvShow* show);
    void playbackStarted();
    void playbackEnded();
    void paused();
    void unpaused();
    void jumped(int seconds);

private:
    QList<QHttpResponse*> listeners;
    const Library& library;
    const VideoPlayer& videoPlayer;
};

#endif // APIPUSHEVENTS_H
