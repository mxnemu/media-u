#include "episode.h"
#include <stdlib.h>
#include <QDebug>

Episode::Episode(nw::Describer *jw, QObject* parent) :
    QObject(parent)
{
    this->describe(jw);
}

Episode::Episode(QString path, QObject *parent) :
    QObject(parent)
{
    addPath(path);
}

Episode::Episode(const MovieFile *path, QObject *parent) :
    QObject(parent)
{
    addPath(path);
}

Episode::~Episode() {
    foreach (const MovieFile* m, files) {
        delete m;
    }
}

void Episode::describe(nw::Describer *de) {
    NwUtils::describe(*de, "watchedDate", watchedDate);

    de->describeValueArray("files", files.length());
    for (int i=0; de->enterNextElement(i); ++i) {
        if (de->isInReadMode()) {
            QString path;
            NwUtils::describeValue(*de, path);
            addPath(new MovieFile(path));
        } else {
            QString path = files.at(i)->path;
            NwUtils::describeValue(*de, path);
        }
    }
}

void Episode::writeDetailed(nw::JsonWriter &jw) {
    NwUtils::describe(jw, "episodeNumber", episodeNumber);
    NwUtils::describe(jw, "showName", showName);
    bool watched = getWatched();
    NwUtils::describe(jw, "watched", watched);
    NwUtils::describe(jw, "watchedDate", watchedDate);
    NwUtils::describe(jw, "numericEpisodeNumber", episodeNumber);

    // TODO don't rely on an abs path for TVSHOWPAGE
    const MovieFile* best = this->bestFile();
    if (best) {
        MovieFile copy = *best;
        NwUtils::describe(jw, "path", copy.path);
        NwUtils::describe(jw, "releaseGroup", copy.releaseGroup);
        NwUtils::describe(jw, "episodeName", copy.episodeName);
        NwUtils::describeValueArray(jw, "tech", copy.techTags);
        NwUtils::describe(jw, "seasonName", copy.seasonName);
        NwUtils::describe(jw, "hashId", copy.hashId);
    }
}

const MovieFile *Episode::getMovieFileForPath(QString path) {
    foreach (const MovieFile* mf, files) {
        if (mf->path == path) {
            return mf;
        }
    }
    return NULL;
}

// TODO implement by checking fansubbers
const MovieFile *Episode::bestFile() const {
    return this->files.empty() ? NULL : this->files.front();
}

QString Episode::getShowName() const {
    return showName;
}

bool Episode::getWatched() const {
    return !watchedDate.isNull();
}

void Episode::setWatched(bool value) {
    bool oldValue = this->getWatched();
    if (value) {
        watchedDate = QDateTime::currentDateTime();
    } else {
        watchedDate = QDateTime();
    }
    emit watchedChanged(oldValue, value);
}

int Episode::getEpisodeNumber() const {
    return this->episodeNumber;
}

QDateTime Episode::getWatchedDate() const
{
    return watchedDate;
}

bool Episode::isSpecial() const {
    return episodeNumber == -2; //MovieFile::SPECIAL;
}


void Episode::addPath(QString path) {
    foreach (const MovieFile* f, files) {
        if (f->path == path) {
            return;
        }
    }
    pushFile(new MovieFile(path));
}

void Episode::addPath(const MovieFile *movieFile) {
    QString path = movieFile->path;
    foreach (const MovieFile* f, files) {
        if (f->path == path) {
            delete movieFile;
            return;
        }
    }
    pushFile(movieFile);
}


void Episode::pushFile(const MovieFile* mf) {
    if (this->files.isEmpty()) {
        this->episodeNumber = mf->numericEpisodeNumber();
        this->showName = mf->showName;
    }
    this->files.push_back(mf);
}
