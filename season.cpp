#include "season.h"
#include <QDebug>
#include "nwutils.h"

Season::Season(QString name) {
    mName = name;
}

Season::~Season()
{
    foreach (MovieFile* file, episodes) {
        if (file) {
            delete file;
        }
    }
}

// TODO put specials into season 0
void Season::exportXbmcLinks(QDir dir) {
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    for (int i=0; i < episodes.length(); ++i) {
        const MovieFile* f = episodes.at(i);
        QString linkName = QString("%1. %2.%3").arg(f->xbmcEpisodeNumber(), f->xbmcEpisodeName(), f->fileExtension());
        QFile::link(f->path(), dir.absoluteFilePath(linkName));
    }
}

void Season::readAsElement(nw::JsonReader &jr) {
    NwUtils::describe(jr, "name", mName);
    jr.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jr.enterNextElement(i); ++i) {
        MovieFile* episode = new MovieFile(&jr);
        addEpisode(episode);
    }
}

void Season::writeAsElement(nw::JsonWriter &jw) {
    NwUtils::describe(jw, "name", mName);
    jw.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        MovieFile* episode = episodes[i];
        episode->describe(&jw);
    }
}

void Season::writeDetailed(nw::JsonWriter &jw) {
    NwUtils::describe(jw, "name", mName);
    jw.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        MovieFile* episode = episodes[i];
        episode->writeDetailed(jw);
    }
}

void Season::addEpisode(MovieFile* episode) {
    if (episode->path().isEmpty()) {
        delete episode;
    } else {
        episodes.append(episode);
    }
}

void Season::addEpisode(const MovieFile& file) {
    addEpisode(file.path());
}

void Season::addEpisode(QString file) {
    if (file.isEmpty() || NULL != getEpisodeForPath(file)) {
        return;
    }    

    episodes.append(new MovieFile(file));
    qDebug() << episodes.back()->releaseGroup() << episodes.back()->episodeNumber() << episodes.back()->showName();
}

QString Season::name() const {
    return mName;
}

int Season::numberOfEpisodes() const
{
    int count = 0;
    for (int i=0; i < episodes.length(); ++i) {
        if (!episodes.at(i)->isSpecial()) {
            ++count;
        }
    }
    return count;
}

int Season::numberOfWatchedEpisodes() const
{
    int count = 0;
    for (int i=0; i < episodes.length(); ++i) {
        if (!episodes.at(i)->isSpecial() && episodes.at(i)->getWatched()) {
            ++count;
        }
    }
    return count;
}

MovieFile* Season::getEpisodeForPath(const QString& path) {
    for (int i=0; i < episodes.length(); ++i) {
        MovieFile* f = episodes[i];
        if (f->path() == path) {
            return f;
        }
    }
    return NULL;
}
