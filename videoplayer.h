#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QProcess>

class VideoPlayer
{
public:
    VideoPlayer();
    virtual void playFile(QString filepath) = 0;

    void togglePause();
    virtual void pause() = 0;
    virtual void unPause() = 0;

    virtual void stop() = 0;

    virtual float incrementSound() = 0; ///< returns the new sound level
    virtual float decrementSound() = 0; ///< returns the new sound level

protected:
    QProcess process;
    bool paused;
};

#endif // VIDEOPLAYER_H
