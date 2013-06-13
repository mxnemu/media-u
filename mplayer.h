#ifndef MPLAYER_H
#define MPLAYER_H

#include "videoplayer.h"

class Mplayer : public VideoPlayer
{
public:
    Mplayer();
    virtual void playFile(QString filepath);

    virtual void pause();
    virtual void unPause();

    virtual void stop();

    virtual float incrementSound();
    virtual float decrementSound();
};

#endif // MPLAYER_H
