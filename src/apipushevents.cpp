#include "apipushevents.h"
#include <qhttprequest.h>
#include <qhttpresponse.h>

ApiPushEvents::ApiPushEvents(const Library &library, const VideoPlayer& videoplayer, QObject *parent) :
    QObject(parent),
    library(library),
    videoPlayer(videoplayer)
{
    connect(&library, SIGNAL(showAdded(TvShow*)), this, SLOT(onShowAdded(TvShow*)));

    //player
    connect(&videoplayer, SIGNAL(playbackStarted()), this, SLOT(playbackStarted()));
    connect(&videoplayer, SIGNAL(playbackEnded()), this, SLOT(playbackEnded()));
    connect(&videoplayer, SIGNAL(unpaused()), this, SLOT(unpaused()));
    connect(&videoplayer, SIGNAL(paused()), this, SLOT(paused()));
    connect(&videoplayer, SIGNAL(jumped(int)), this, SLOT(jumped(int)));
}

bool ApiPushEvents::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) {
    if (req->path() == "/api/events/all") {
        listeners.push_back(resp);
        connect(resp, SIGNAL(done()), this, SLOT(listenerTerminated()));

        resp->setHeader("Content-Type", "text/event-stream");
        resp->setHeader("Cache-Control", "no-cache");
        resp->setHeader("Connection", "keep-alive");
        resp->writeHead(200);
        return true;
    }
    return false;
}

void ApiPushEvents::listenerTerminated() {
    this->listeners.removeOne(dynamic_cast<QHttpResponse*>(this->sender()));
}

void ApiPushEvents::onShowAdded(TvShow *show) {
    std::stringstream ss;
    nw::JsonWriter jw(ss);
    show->writeAsListingItem(&jw);
    jw.close();
    this->sendToListeners(ss.str().data(), "showAdded");
}

void ApiPushEvents::playbackStarted() {
    this->sendToListeners(videoPlayer.getNowPlaying().toJson(), "playbackStarted");
}

void ApiPushEvents::playbackEnded() {
    this->sendToListeners("{}", "playbackEnded");
}

void ApiPushEvents::paused() {
    this->sendToListeners("{}", "paused");
}

void ApiPushEvents::unpaused() {
    this->sendToListeners("{}", "unpaused");
}

void ApiPushEvents::jumped(int seconds) {
    this->sendToListeners(QString("{\"seconds\":%1}").arg(seconds), "jumped");
}

void ApiPushEvents::sendToListeners(const QString& message, const QString& event) {
    for (int i=0; i < listeners.length(); ++i) {
        if (!event.isNull()) {
            this->listeners[i]->write(QString("event: "));
            this->listeners[i]->write(event);
            this->listeners[i]->write(QString("\n"));
        }
        this->listeners[i]->write(QString("data: "));
        this->listeners[i]->write(QString(message).replace("\n", "\ndata: "));
        this->listeners[i]->write(QString("\n\n"));
        this->listeners[i]->flush();
    }
}

