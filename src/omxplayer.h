#ifndef OMXPLAYER_H
#define OMXPLAYER_H

#include "videoplayer.h"

class Omxplayer : public VideoPlayer
{
public:
    Omxplayer(Library& library);

    virtual int playFileImpl(QString filepath, const TvShowPlayerSettings &);

    virtual void pause();
    virtual void unPause();

    virtual void stop();

    virtual void backwards(const int seconds = 5);
    virtual void forwards(const int seconds = 5);

    virtual float incrementVolume();
    virtual float decrementVolume();
};

#endif // OMXPLAYER_H
