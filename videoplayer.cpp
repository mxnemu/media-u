#include "videoplayer.h"
#include <server.h>

VideoPlayer::VideoPlayer(QObject* parent) //:
    //QObject(parent)
{
    paused = false;
}

VideoPlayer::~VideoPlayer() {
    this->process.kill();
}

void VideoPlayer::togglePause() {
    if (paused) {
        unPause();
    } else {
        pause();
    }
}

bool VideoPlayer::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) {
    if (req->path().startsWith("/api/player/play") && !req->url().query().isEmpty()) {
        stringstream ss;
        ss << req->url().query(QUrl::FullyDecoded).toStdString();
        QString episode;
        nw::JsonReader jr(ss);
        NwUtils::describe(jr, "filename", episode);
        jr.close();

        int error = this->playFile(episode);
        if (this->process.state() == QProcess::Running) {
            Server::simpleWrite(resp, 200, QString("{\"status\":\"playback started\"}"));
        } else {
            Server::simpleWrite(resp, 500, QString("{\"status\":\"could not start playback\", \"error\":%1}").arg(error));
        }
    } else if (req->path() == "/api/player/stop") {
        this->stop();
        Server::simpleWrite(resp, 200, "{\"status\":\"stopped\"}");
    } else if (req->path() == "/api/player/togglePause") {
        this->togglePause();
        QString status = paused ? "paused" : "unPaused";
        Server::simpleWrite(resp, 200, QString("{\"status\":\"%1\"}").arg(status));
    } else if (req->path() == "/api/player/unPause") {
        this->unPause();
        Server::simpleWrite(resp, 200, "{\"status\":\"unPaused\"}");
    } else if (req->path() == "/api/player/pause") {
        this->pause();
        Server::simpleWrite(resp, 200, "{\"status\":\"paused\"}");
    } else if (req->path() == "/api/player/backwards") {
        this->backwards();
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}");
    } else if (req->path() == "/api/player/forwards") {
        this->forwards();
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}");
    } else if (req->path() == "/api/player/incrementVolume") {
        float volume = this->incrementVolume();
        Server::simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"volume\":%1}").arg(volume));
    } else if (req->path() == "/api/player/decrementVolume") {
        float volume = this->decrementVolume();
        Server::simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"volume\":%1}").arg(volume));
    } else if (req->path() == "/api/player/pauseStatus") {
        QString status = paused ? "paused" : "unPaused";
        Server::simpleWrite(resp, 200, QString("{\"status\":\"%1\"}").arg(status));
    } else {
        return customHandleApiRequest();
    }
    return true;
}
