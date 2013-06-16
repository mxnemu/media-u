#ifndef OMXPLAYER_H
#define OMXPLAYER_H

#include "videoplayer.h"

class Omxplayer : public VideoPlayer
{
public:
    Omxplayer();

    virtual int playFile(QString filepath);

    virtual void pause();
    virtual void unPause();

    virtual void stop();

    virtual void backwards();
    virtual void forwards();

    virtual float incrementVolume();
    virtual float decrementVolume();
};

#endif // OMXPLAYER_H
