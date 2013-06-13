#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QProcess>
#include <qhttpconnection.h>

class VideoPlayer
{
public:
    VideoPlayer();
    virtual void playFile(QString filepath) = 0;

    void togglePause();
    virtual void pause() = 0;
    virtual void unPause() = 0;

    virtual void stop() = 0;

    virtual void backwards() = 0;
    virtual void forwards() = 0;

    virtual float incrementVolume() = 0; ///< returns the new sound level
    virtual float decrementVolume() = 0; ///< returns the new sound level

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);

protected:
    virtual bool customHandleApiRequest() { return false; }

    QProcess process;
    bool paused;
};

#endif // VIDEOPLAYER_H
