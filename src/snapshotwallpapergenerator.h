#ifndef SNAPSHOTWALLPAPERGENERATOR_H
#define SNAPSHOTWALLPAPERGENERATOR_H

#include "wallpaperdownloadclient.h"
#include "thumbnailcreator.h"
#include "metadataparser.h"

namespace SnapshotWallpaperGenerator {
using WallpaperDownload::Entry;
using WallpaperDownload::SearchResult;

class Client : public WallpaperDownload::Client {
    Q_OBJECT
public:
    Client(MetaDataParser& mp, ThumbnailCreator& tc);

    virtual SearchResult fetchPostsBlocking(const TvShow* show, int = 1);
    virtual void downloadResults(QDir directory, const QList<Entry> &entries, bool onlyTheBest);

    void generated(QString file); ///< fires the signal

protected:
    virtual inline Entry parseEntry(nw::Describer*) {return Entry();}
    virtual inline SearchResult parseSearchResult(std::stringstream&, int) {return SearchResult();}
    virtual inline CURL* curlClient(QString, CurlResult&, const unsigned int = 1) {return NULL;}

private slots:
    void wallpaperReady(QByteArray);

private:
    MetaDataParser& metaDataParser;
    ThumbnailCreator& thumbnailCreator;
    int outstandingWallpapers;
};

class ThumbnailCreationData {
public:
    QDir dir;
    QString filename;
    Client* client;
};

} // namespace

#endif // SNAPSHOTWALLPAPERGENERATOR_H
