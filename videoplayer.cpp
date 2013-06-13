#include "videoplayer.h"
#include <server.h>

VideoPlayer::VideoPlayer()
{
    paused = false;
}

void VideoPlayer::togglePause()
{
    if (paused) {
        unPause();
    } else {
        pause();
    }
    paused = !paused;
}

bool VideoPlayer::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) {
    if (req->path() == "/api/player/stop") {
        this->stop();
        Server::simpleWrite(resp, 200, "{\"status\":\"stopped\"}");
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
