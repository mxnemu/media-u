#include "library.h"
#include "nwutils.h"
#include <QDebug>
#include "server.h"
#include "directoryscanner.h"
#include "tvshowscanner.h"
#include <QStandardPaths>
#include "moviefile.h"

Library::Library(QString path, QObject *parent) :
    QObject(parent),
    directory(path),
    mFilter(tvShows, directory),
    searchThread(NULL)
{
    if (!directory.exists() && !QDir::root().mkpath(directory.absolutePath())) {
        qDebug() << "could not create library dir";
    }
    addWallpaperDownloader(new Moebooru::Client(("http://konachan.com")));
    addWallpaperDownloader(new Moebooru::Client(("https://yande.re")));
    addWallpaperDownloader(new Gelbooru::Client());

    connect(&malapiClient, SIGNAL(updateFinished()),
            this, SLOT(fetchingFinished()));

    fileSystemWatcher = new QFileSystemWatcher(this);
    connect(fileSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChangedInSearchDirectory(QString)));
    connect(fileSystemWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(fileChangedInSearchDirectory(QString)));
}

Library::~Library() {
    Mal::Thread* malThread = malClient.getActiveThread();
    foreach (WallpaperDownload::FetchThread* t, runningWallpaperDownloaders) {
        t->terminate();
    }
    searchThread->terminate();
    if (malThread) malThread->terminate();

    foreach (WallpaperDownload::FetchThread* t, runningWallpaperDownloaders) {
        t->wait();
    }
    searchThread->wait();
    if (malThread) malThread->wait();

}

void Library::initMalClient(QString malConfigFilepath) {
    malClient.init(malConfigFilepath);
    connect(&malClient, SIGNAL(fetchingFinished()),
            this, SLOT(fetchingFinished()));
}

bool Library::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    if (req->path().startsWith("/api/library/filter")) {
        return filter().handleApiRequest(req, resp);
    } else if (req->path().startsWith("/api/library/tvshow/")) {
        const int prefixOffset = sizeof("/api/library/tvshow/")-1;
        const QString encodedPath = req->url().path(QUrl::EncodeDelimiters);
        QString encodedShowName = encodedPath.mid(prefixOffset, encodedPath.indexOf('/', prefixOffset)-prefixOffset);
        QString showName = QUrl::fromPercentEncoding(encodedShowName.toUtf8());
        qDebug() << showName;

        TvShow* show = existingTvShow(showName);
        if (show) {
            int urlPrefixOffset = encodedPath.indexOf('/', prefixOffset);
            show->handleApiRequest(urlPrefixOffset, req, resp);
        } else {
            Server::simpleWrite(resp, 400, "{\"error\":\"invalid show name\"}");
        }
    } else if (req->path().startsWith("/api/library/randomWallpaper")) {
        // TODO change abs img path to server url
        Server::simpleWrite(resp, 200, QString("{\"image\":\"%1\"}").arg(filter().getRandomWallpaper()), mime::json);
    } else if (req->path().startsWith("/api/library/toggleWatched")) {
        Episode* episode = filter().getEpisodeForPath(req->url().query(QUrl::FullyDecoded));
        if (episode) {
            episode->setWatched(!episode->getWatched());
            Server::simpleWrite(resp, 200, QString("{\"watched\":%1}").arg(episode->getWatched() ? "true" : "false"), mime::json);
        } else {
            Server::simpleWrite(resp, 400, QString("{\"error\":\"Episode not found\"}"), mime::json);
        }
    } else if (req->path().startsWith("/api/library/movieFileMetaData")) {
        QString path = req->url().query(QUrl::FullyDecoded);
        Episode* ep = filter().getEpisodeForPath(path);
        if (ep) {
            MetaData data = metaDataParser->parse(path);
            Server::simpleWrite(resp, 200, data.toJson(), mime::json);
        }
        Server::simpleWrite(resp, 404, "File for path does not exist, or is not in Library" "text/plain");
    } else {
        return false;
    }
    return true;
}

TvShow& Library::tvShow(const QString name) {
    for (QList<TvShow*>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        if ((*it)->name().compare(name, Qt::CaseInsensitive) == 0) {
            return *(it.i->t());
        }
    }
    foreach (TvShow* show, tvShows) {
        if (show->getSynonyms().contains(name, Qt::CaseInsensitive)) {
            return *show;
        }
    }
    this->tvShows.push_back(new TvShow(name, this));
    TvShow* show = this->tvShows.back();
    emit showAdded(show);
    connect(&show->episodeList(), SIGNAL(beforeWatchCountChanged(int,int)), this, SIGNAL(beforeWatchCountChanged(int,int)));
    return *show;
}

TvShow* Library::existingTvShow(const QString name) {
    for (QList<TvShow*>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        if ((*it)->name() == name) {
            return it.i->t();
        }
    }
    return NULL;
}

const LibraryFilter& Library::filter() const
{
    return mFilter;
}

void Library::importTvShowEpisode(QString episodePath) {
    const MovieFile* movieFile = new MovieFile(episodePath);
    if (!movieFile->showName.isEmpty()) {
        TvShow& show = this->tvShow(movieFile->showName);
        show.importMovieFile(movieFile); // takes ownage
    } else {
        qDebug() << "could not import (no show name parsed):" << movieFile->showName;
        delete movieFile;
    }
}

/*
void Library::xbmcLinkExport(QDir outputDir) {
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    for (QList<TvShow*>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        (*it)->exportXbmcLinks((*it)->directory(outputDir));
    }
}
*/

void Library::fetchMetaData() {
    malClient.fetchShows(tvShows, *this);
    //malapiClient.startUpdate(tvShows, directory);
}

void Library::startWallpaperDownloaders() {
    if (getWallpaperDownloadRunning()) {
        foreach (WallpaperDownload::FetchThread* ft, runningWallpaperDownloaders) { 
            if (ft->isRunning()) {
                ft->append(filter().all());
            }
        }
        return;
    }
    for (int i=0; i < wallpaperDownloaders.size(); ++i) {
        WallpaperDownload::Client* downloader = wallpaperDownloaders[i];
        WallpaperDownload::FetchThread* ft = new WallpaperDownload::FetchThread(*downloader, filter().all(), directory, this);
        //connect(this, SIGNAL(destroyed()), ft, SLOT(terminate()));
        connect(ft, SIGNAL(finished()), this, SLOT(wallpaperDownloaderFinished()));
        connect(ft, SIGNAL(finished()), ft, SLOT(deleteLater()));
        runningWallpaperDownloaders.push_back(ft);
        ft->start(QThread::LowPriority);
    }
}

void Library::startSearch() {
    startSearch(searchDirectories);
}

void Library::startSearch(const QList<SearchDirectory> dirs) {
    if (this->searchThread) {
        if (this->searchThread->isFinished()) {
            QThread* t = this->searchThread;
            this->searchThread = NULL;
            delete t;
        } else {
            return;
        }
    }
    DirectoryScanner* scanner = new DirectoryScanner();
    scanner->addScanner(new TvShowScanner(*this));
    this->searchThread = new DirectoryScannerThread(scanner, dirs, this);
    connect(searchThread, SIGNAL(done()), this, SIGNAL(searchFinished()));
    connect(searchThread, SIGNAL(machingFile(QString)), this, SLOT(importTvShowEpisode(QString)));
    connect(searchThread, SIGNAL(done()), this, SLOT(startWallpaperDownloaders()));
    connect(searchThread, SIGNAL(done()), this, SLOT(fetchMetaData()));
    this->searchThread->start(QThread::HighPriority);
}

Library::searchStatus Library::getSearchStatus() {
    if (this->searchThread) {
        return this->searchThread->isRunning() ? inProgress : done;
    }
    return notStarted;
}

bool Library::getWallpaperDownloadRunning() {
    return !runningWallpaperDownloaders.empty();
}

const QList<SearchDirectory>& Library::getSearchDirectories() const {
    return searchDirectories;
}

bool Library::addSearchDirectory(SearchDirectory dir) {
    if (getSearchDirectory(dir.dir.absolutePath()) == NULL) {
        searchDirectories.append(dir);
        fileSystemWatcher->addPath(dir.dir.absolutePath());
        qDebug() << fileSystemWatcher->directories();
        return true;
    }
    return false;
}

void Library::addWallpaperDownloader(WallpaperDownload::Client* client) {
    wallpaperDownloaders.push_back(client);
    client->setParent(this);
    connect(client, SIGNAL(wallpaperDownloaded(QString)), this, SIGNAL(wallpaperDownloaded(QString)));
}

SearchDirectory *Library::getSearchDirectory(const QString path) {
    for (int i=0; i < searchDirectories.length(); ++i) {
        if (searchDirectories.at(i).dir.absolutePath() == QDir(path).absolutePath()) {
            return &searchDirectories[i];
        }
    }
    return NULL;
}

bool Library::removeSearchDirectory(QString path) {
    for (int i=0; i < searchDirectories.length(); ++i) {
        if (searchDirectories.at(i).dir.absolutePath() == QDir(path).absolutePath()) {
            searchDirectories.removeAt(i);
            return true;
        }
    }
    return false;
}

void Library::generateFrenchises() {
    foreach (TvShow* show, tvShows) {
        show->syncRelations(*this);
    }
    foreach (const TvShow* show, tvShows) {
        addToFrenchise(show);
    }
    foreach (Franchise* franchise, franchises) {
        franchise->generateName();
    }
}

void Library::fileChangedInSearchDirectory(QString path) {
    SearchDirectory* sd = getSearchDirectory(path);
    if (sd) {
        startSearch(QList<SearchDirectory>() << SearchDirectory(sd->dir));
    }
}

void Library::addToFrenchise(const TvShow* show) {
    bool added = false;
    foreach (Franchise* f, franchises) {
        if (f->hasRelationTo(show)) {
            f->addTvShow(show);
            added = true;
            break;
        }
    }
    if (!added) {
        Franchise* newFranchise = new Franchise(this);
        newFranchise->addTvShow(show);
        franchises.push_back(newFranchise);
    }
}

void Library::fetchingFinished() {
    qDebug() << "finished mal fetching, writing things now";
    generateFrenchises();
    this->write();
    qDebug() << "writing done!";
}

void Library::wallpaperDownloaderFinished() {
    runningWallpaperDownloaders.removeOne(dynamic_cast<WallpaperDownload::FetchThread*>(sender()));
    if (!getWallpaperDownloadRunning()) {
        emit wallpaperDownloadersFinished();
    }
}

void Library::readAll() {
    QString filepath = directory.absoluteFilePath("library.json");
    if (QFile(filepath).exists()) {
        nw::JsonReader jr(filepath.toStdString());


        jr.describeArray("searchDirectories", "directory", searchDirectories.length());
        for (int i=0; jr.enterNextElement(i); ++i) {
            QString dirpath;
            bool enabled = false;
            NwUtils::describe(jr, "path", dirpath);
            NwUtils::describe(jr, "enabled", enabled);
            addSearchDirectory(SearchDirectory(dirpath, enabled));
        }

        jr.describeValueArray("tvShows", tvShows.length());
        for (int i=0; jr.enterNextElement(i); ++i) {
            std::string name;
            jr.describeValue(name);
            TvShow& show = tvShow(QString(name.data()));

            QDir showDir = show.directory(directory);
            if (!showDir.exists()) {
                qDebug() << "show does not have a directory: " << show.name();
                break;
            }
            show.read(showDir);
        }
        jr.close();
    } else {
        QStringList defaultDirs = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation);
        defaultDirs.append(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation));

        for (int i=0; i < defaultDirs.length(); ++i) {
            QDir dir = QDir(defaultDirs.at(i));
            if (dir.exists()) {
                searchDirectories.append(SearchDirectory(dir));
            }
        }
    }
    generateFrenchises();
}

void Library::write() {
    if (directory.exists()) {
        nw::JsonWriter jw(directory.absoluteFilePath("library.json").toStdString());
        jw.describeArray("searchDirectories", "directory", searchDirectories.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            QString dirpath = searchDirectories.at(i).dir.path();
            NwUtils::describe(jw, "path", dirpath);
            NwUtils::describe(jw, "enabled", searchDirectories[i].enabled);
        }
        jw.describeValueArray("tvShows", tvShows.length());
        for (int i=0; jw.enterNextElement(i); ++i) {
            TvShow* show = tvShows[i];
            std::string name = show->name().toStdString();
            jw.describeValue(name);

            QDir showDir = show->directory(directory);
            if (!showDir.exists() && !directory.mkdir(showDir.dirName())) {
                // TODO
                qDebug() << "TODO thow error can not write library" << showDir.absolutePath();
                continue;
            }
            nw::JsonWriter showJw(showDir.absoluteFilePath("tvShow.json").toStdString());
            show->write(showJw);
            showJw.close();
        }
        jw.close();
    }
}


MetaDataParser *Library::getMetaDataParser() const
{
    return metaDataParser;
}

void Library::setMetaDataParser(MetaDataParser *value)
{
    metaDataParser = value;
}

QDir Library::getDirectory() const
{
    return directory;
}
