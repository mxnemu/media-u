#ifndef MPLAYER_H
#define MPLAYER_H

#include "videoplayer.h"
#include <QObject>
#include "config.h"

class Mplayer : public VideoPlayer
{
    Q_OBJECT
public:
    Mplayer(Library& library, const MplayerConfig& config);
    virtual ~Mplayer();

    virtual void stop();

    virtual void backwards(const int seconds = 5);
    virtual void forwards(const int seconds = 5);

    virtual float incrementVolume();
    virtual float decrementVolume();

    void convertSnapshot(QString snapshotName);
protected:
    virtual bool playFileImpl(QString filepath, const TvShowPlayerSettings &settings);
    virtual void pauseImpl();
    virtual void unPauseImpl();

    const MplayerConfig& config;

private slots:
    void onProcessFinished(int exitCode);
    void onProcessOutput();
};

#endif // MPLAYER_H
