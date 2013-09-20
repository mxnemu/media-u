#ifndef MPLAYER_H
#define MPLAYER_H

#include "videoplayer.h"
#include <QObject>

class Mplayer : public VideoPlayer
{
    Q_OBJECT
public:
    Mplayer(Library& library);
    virtual ~Mplayer();

    virtual void pause();
    virtual void unPause();

    virtual void stop();

    virtual void backwards(const int seconds = 5);
    virtual void forwards(const int seconds = 5);

    virtual float incrementVolume();
    virtual float decrementVolume();

protected:
    virtual int playFileImpl(QString filepath);

private slots:
    void onProcessFinished(int exitCode);
    void onProcessOutput();
};

#endif // MPLAYER_H
