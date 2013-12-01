#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <qhttpconnection.h>
#include <QFileSystemWatcher>
#include "tvshow.h"
#include "malclient.h"
#include "malapidotcomclient.h"
#include "moebooruclient.h"
#include "gelbooruclient.h"
#include "metadataparser.h"
#include "libraryfilter.h"
#include "searchdirectory.h"
#include "franchise.h"

class DirectoryScannerThread;

class Library : public QObject
{
    Q_OBJECT
public:

    enum searchStatus {
        notStarted,
        inProgress,
        done
    };

    explicit Library(QString path, QObject *parent = 0);
    void initMalClient(QString malConfigFilepath);

    bool handleApiRequest(QHttpRequest* req, QHttpResponse* resp);

    TvShow& tvShow(const QString name);
    TvShow* existingTvShow(const QString name);
    LibraryFilter& filter();

    //void xbmcLinkExport(QDir outputDir);
    void write();
    void readAll();

    QDir getDirectory() const;

    void startSearch();
    void startSearch(const QList<SearchDirectory> dirs);
    Library::searchStatus getSearchStatus();
    bool getWallpaperDownloadRunning();

    const QList<SearchDirectory>& getSearchDirectories() const;
    bool addSearchDirectory(SearchDirectory dir);
    SearchDirectory* getSearchDirectory(QString path);
    bool removeSearchDirectory(QString path);

    void addToFrenchise(const TvShow *show);

    MetaDataParser *getMetaDataParser() const;
    void setMetaDataParser(MetaDataParser *value);

signals:
    void showAdded(TvShow* show);
    void searchFinished();
    void wallpaperDownloadersFinished();
    void wallpaperDownloaded(QString);
    
public slots:
    void importTvShowEpisode(QString episodePath);
    void startWallpaperDownloaders();
    void fetchMetaData();
    void generateFrenchises();

    void fileChangedInSearchDirectory(QString);

private slots:
    void fetchingFinished();
    void wallpaperDownloaderFinished();

private:
    QDir directory;
    QList<TvShow*> tvShows;
    QList<Franchise*> franchises;
    QList<SearchDirectory> searchDirectories;

    MalClient malClient;
    MalApiDotCom::Client malapiClient;
    LibraryFilter mFilter;
    MetaDataParser* metaDataParser;

    QList<WallpaperDownload::Client*> wallpaperDownloaders;
    QList<WallpaperDownload::FetchThread*> runningWallpaperDownloaders;
    DirectoryScannerThread* searchThread;
    QFileSystemWatcher* fileSystemWatcher;
};

#endif // LIBRARY_H
