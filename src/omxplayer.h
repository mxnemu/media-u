#ifndef OMXPLAYER_H
#define OMXPLAYER_H

#include "videoplayer.h"

class Omxplayer : public VideoPlayer
{
public:
    Omxplayer(Library& library);

    virtual bool playFileImpl(QString filepath, const TvShowPlayerSettings &);

    virtual void pauseImpl();
    virtual void unPauseImpl();

    virtual void stop();

    virtual void backwards(const int = 5);
    virtual void forwards(const int = 5);

    virtual float incrementVolume();
    virtual float decrementVolume();
};

#endif // OMXPLAYER_H
