#include "episode.h"
#include <stdlib.h>
#include <QDebug>
#include "utils.h"

Episode::Episode(nw::Describer *jw, QObject* parent) :
    QObject(parent),
    episodeNumber(VideoFile::INVALID)
{
    this->describe(jw);
}

Episode::Episode(QString path, QObject *parent) :
    QObject(parent),
    episodeNumber(VideoFile::INVALID)
{
    addPath(path);
}

Episode::Episode(const VideoFile *path, QObject *parent) :
    QObject(parent),
    episodeNumber(VideoFile::INVALID)
{
    addPath(path);
}

Episode::~Episode() {
    foreach (const VideoFile* m, files) {
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
            addPath(new VideoFile(path));
        } else {
            QString path = files.at(i)->path;
            NwUtils::describeValue(*de, path);
        }
    }
}

void Episode::writeDetailed(nw::JsonWriter &jw, const QStringList& releaseGroupPreference) {
    NwUtils::describe(jw, "showName", showName);
    bool watched = getWatched();
    NwUtils::describe(jw, "watched", watched);
    NwUtils::describe(jw, "watchedDate", watchedDate);
    NwUtils::describe(jw, "numericEpisodeNumber", episodeNumber);

    // TODO don't rely on an abs path for TVSHOWPAGE
    const VideoFile* best = this->bestFile(releaseGroupPreference);
    if (best) {
        VideoFile copy = *best;
        bool exists = copy.exists();
        NwUtils::describe(jw, "path", copy.path);
        NwUtils::describe(jw, "episodeNumber", copy.episodeNumber);
        NwUtils::describe(jw, "releaseGroup", copy.releaseGroup);
        NwUtils::describe(jw, "episodeName", copy.episodeName);
        NwUtils::describeValueArray(jw, "tech", copy.techTags);
        NwUtils::describe(jw, "seasonName", copy.seasonName);
        NwUtils::describe(jw, "hashId", copy.hashId);
        NwUtils::describe(jw, "exists", exists);
    }
}

const VideoFile *Episode::getMovieFileForPath(QString path) {
    foreach (const VideoFile* mf, files) {
        if (mf->path == path) {
            return mf;
        }
    }
    return NULL;
}

const VideoFile *Episode::bestFile(const QStringList& releaseGroupPreference) const {
    std::pair<const VideoFile*, int> best = std::pair<const VideoFile*, int>(NULL,-1);
    const int worst = releaseGroupPreference.length();
    foreach (const VideoFile* file, files) {
        if (!file) {
            continue;
        }
        int score = releaseGroupPreference.indexOf(file->releaseGroup);
        score = -1 == score ? worst : score;
        score += worst * !file->exists();
        if (best.second == -1 || score < best.second) {
            best.first = file;
            best.second = score;
        }
    }
    return best.first;
}

QString Episode::getShowName() const {
    return showName;
}

bool Episode::getWatched() const {
    return !watchedDate.isNull();
}

void Episode::setWatched(bool value) {
    bool oldValue = this->getWatched();
    emit beforeWatchedChanged(value, oldValue);
    if (value) {
        if (!oldValue) {
            watchedDate = QDateTime::currentDateTime();
        }
    } else {
        watchedDate = QDateTime();
    }
    emit watchedChanged(oldValue, value);
}

float Episode::getEpisodeNumber() const {
    return this->episodeNumber;
}

QDateTime Episode::getWatchedDate() const
{
    return watchedDate;
}

QStringList Episode::releaseGroups() const {
    QStringList groups;
    foreach (const VideoFile* mf, files) {
        if (!groups.contains(mf->releaseGroup)) {
            groups.push_back(mf->releaseGroup);
        }
    }
    return groups;
}

bool Episode::isSpecial() const {
    return episodeNumber == VideoFile::SPECIAL;
}


void Episode::addPath(QString path) {
    foreach (const VideoFile* f, files) {
        if (f->path == path) {
            return;
        }
    }
    pushFile(new VideoFile(path));
}

void Episode::addPath(const VideoFile *movieFile) {
    QString path = movieFile->path;
    foreach (const VideoFile* f, files) {
        if (f->path == path) {
            delete movieFile;
            return;
        }
    }
    pushFile(movieFile);
}


void Episode::pushFile(const VideoFile* mf) {
    if (this->files.isEmpty()) {
        this->episodeNumber = mf->numericEpisodeNumber();
        this->showName = mf->showName;
    }
    this->files.push_back(mf);
}

QList<const VideoFile*> Episode::missingFiles() const {
    return Utils::filter(files, [](const VideoFile*const& f) -> bool {
        return !f->exists();
    });
}
