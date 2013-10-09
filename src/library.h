#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <qhttpconnection.h>
#include "tvshow.h"
#include "moviefile.h"
#include "malclient.h"
#include "malapidotcomclient.h"
#include "moebooruclient.h"
#include "gelbooruclient.h"
#include "libraryfilter.h"
#include "searchdirectory.h"

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

    void xbmcLinkExport(QDir outputDir);
    void write();
    void readAll();

    QDir getDirectory() const;

    void startSearch();
    Library::searchStatus getSearchStatus();
    bool getWallpaperDownloadRunning();

    const QList<SearchDirectory>& getSearchDirectories() const;
    bool addSearchDirectory(SearchDirectory dir);
    SearchDirectory* getSearchDirectory(QString path);
    bool removeSearchDirectory(QString path);

signals:
    void showAdded(TvShow* show);
    void searchFinished();
    void wallpaperDownloadersFinished();
    void wallpaperDownloaded(QString);
    
public slots:
    void importTvShowEpisode(QString episodePath);
    void startWallpaperDownloaders();
    void fetchMetaData();

private slots:
    void fetchingFinished();
    void wallpaperDownloaderFinished();

private:
    QDir directory;
    QList<TvShow*> tvShows;
    QList<MovieFile*> movies;
    QList<SearchDirectory> searchDirectories;

    MalClient malClient;
    MalApiDotCom::Client malapiClient;
    LibraryFilter mFilter;
    // TODO put into an array
    QList<WallpaperDownload::Client*> wallpaperDownloaders;
    QList<WallpaperDownload::FetchThread*> runningWallpaperDownloaders;
    DirectoryScannerThread* searchThread;
};

#endif // LIBRARY_H
