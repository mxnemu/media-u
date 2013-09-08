#include "library.h"
#include "nwutils.h"
#include <QDebug>
#include "server.h"
#include "directoryscanner.h"
#include "tvshowscanner.h"

Library::Library(QString path, QObject *parent) :
    QObject(parent),
    directory(path),
    mFilter(tvShows),
    konachanClient("http://konachan.com"),
    yandereClient("https://yande.re")
{
    if (!directory.exists() && !QDir::root().mkpath(directory.absolutePath())) {
        qDebug() << "could not create library dir";
    }
}

void Library::initMalClient(QString malConfigFilepath) {
    malClient.init(malConfigFilepath);
}

bool Library::handleApiRequest(QHttpRequest *req, QHttpResponse *resp)
{
    if (req->path().startsWith("/api/library/filter")) {
        return filter().handleApiRequest(req, resp);
    } else if(req->path().startsWith("/api/library/randomWallpaper")) {
        // TODO change abs img path to server url
        Server::simpleWrite(resp, 200, QString("{\"image\":\"%1\"}").arg(randomWallpaperPath()));
    } else {
        return false;
    }
    return true;
}

QString Library::randomWallpaperPath() const {
    // TODO debug test file; Impl actual fn
    return QString("/home/nehmulos/Downloads/test-wall.jpg");
}

TvShow& Library::tvShow(const QString name) {
    for (QList<TvShow>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        if (it->name().compare(name, Qt::CaseInsensitive) == 0) {
            return it.i->t();
        }
    }
    this->tvShows.push_back(TvShow(name));
    return this->tvShows.back();
}

TvShow* Library::existingTvShow(const QString name) {
    for (QList<TvShow>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        if (it->name() == name) {
            return &it.i->t();
        }
    }
    return NULL;
}

LibraryFilter &Library::filter()
{
    return mFilter;
}

void Library::importTvShowEpisode(QString episodePath) {
    MovieFile episode(episodePath);
    TvShow& show = this->tvShow(episode.showName());
    show.importEpisode(episode);
}

void Library::xbmcLinkExport(QDir outputDir) {
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    for (QList<TvShow>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        it->exportXbmcLinks(it->directory(outputDir));
    }
}

void Library::fetchMetaData() {
    malClient.fetchShows(tvShows, directory);
    connect(&malClient, SIGNAL(fetchingFinished()),
            this, SLOT(fetchingFinished()));
}

void Library::downloadWallpapers() {
    Moebooru::FetchThread* ft = new Moebooru::FetchThread(konachanClient, filter().all(), directory);
    ft->start(QThread::LowPriority);
    connect(ft, SIGNAL(finished()), ft, SLOT(deleteLater()));
}

void Library::startSearch() {
    DirectoryScanner scanner;
    scanner.addScanner(new TvShowScanner(*this));
    for (int i=0; i < searchDirectories.length(); ++i) {
        scanner.scan(searchDirectories.at(i).dir.absolutePath());
    }
    //library.write();
}

const QList<SearchDirectory>& Library::getSearchDirectories() const {
    return searchDirectories;
}

bool Library::addSearchDirectory(SearchDirectory dir) {
    if (getSearchDirectory(dir.dir.absolutePath()) == NULL) {
        searchDirectories.append(dir);
        return true;
    }
    return false;
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

void Library::fetchingFinished() {
    qDebug() << "finished mal fetching, writing things now";
    this->write();
    qDebug() << "writing done!";
}

void Library::readAll() {
    if (directory.exists()) {
        nw::JsonReader jr(directory.absoluteFilePath("library.json").toStdString());


        jr.describeArray("searchDirectories", "directory", searchDirectories.length());
        for (int i=0; jr.enterNextElement(i); ++i) {
            QString dirpath;
            bool enabled = false;
            NwUtils::describe(jr, "path", dirpath);
            NwUtils::describe(jr, "enabled", enabled);
            searchDirectories.append(SearchDirectory(dirpath, enabled));
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
    }
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
            TvShow& show = tvShows[i];
            std::string name = show.name().toStdString();
            jw.describeValue(name);

            QDir showDir = show.directory(directory);
            if (!showDir.exists() && !directory.mkdir(show.name())) {
                // TODO
                qDebug() << "TODO thow error can not write library";
                break;
            }
            nw::JsonWriter showJw(showDir.absoluteFilePath("tvShow.json").toStdString());
            show.write(showJw);
            showJw.close();
        }
        jw.close();
    }
}

QDir Library::getDirectory() const
{
    return directory;
}

