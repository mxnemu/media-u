#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QProcess>
#include <qhttpconnection.h>
#include <QObject>
#include "metadataparser.h"
#include "thumbnailcreator.h"
#include "library.h"

class VideoPlayer : public QObject
{
   Q_OBJECT // TODO build breaks here when trying to inherit qobject
public:
    explicit VideoPlayer(Library &library, QObject *parent = NULL);
    virtual ~VideoPlayer();
    virtual int playFile(QString filepath) = 0;

    void togglePause();
    virtual void pause() = 0;
    virtual void unPause() = 0;

    virtual void stop() = 0;

    virtual void jumpTo(int second);
    virtual void backwards(const int seconds = 5) = 0;
    virtual void forwards(const int seconds = 5) = 0;

    virtual float incrementVolume() = 0; ///< returns the new sound level
    virtual float decrementVolume() = 0; ///< returns the new sound level

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);

    const MetaDataParser *getMetaDataParser() const;
    void setMetaDataParser(const MetaDataParser *value);

    const ThumbnailCreator *getThumbnailCreator() const;
    void setThumbnailCreator(const ThumbnailCreator *value);

    QStringList getPlaylist() const;
    void setPlaylist(const QStringList &value);

    void resetPlayingStatus();
signals:
    void playbackEndedNormally();
    void playbackCanceled();

protected:
    virtual bool customHandleApiRequest() { return false; }

    Library& library;
    QString playingFile;
    const MetaDataParser* metaDataParser;
    const ThumbnailCreator* thumbnailCreator;
    QProcess process;
    bool paused;
    int progress; ///< in seconds

    QStringList playlist;

public slots:
    void onThumbnailCreated(const QByteArray img);

private slots:
    void onPlaybackEndedNormally();
    void onPlaybackCanceled();
};

#endif // VIDEOPLAYER_H
