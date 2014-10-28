#include "episodelist.h"
#include <QDebug>
#include "nwutils.h"

EpisodeList::EpisodeList(QObject* parent) : QObject(parent) {
}

EpisodeList::~EpisodeList() {
}

void EpisodeList::exportXbmcLinks(QDir) {
    /*
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    for (int i=0; i < episodes.length(); ++i) {
        const Episode* f = episodes.at(i);
        QString linkName = QString("%1. %2.%3").arg(f->xbmcEpisodeNumber(), f->xbmcEpisodeName(), f->fileExtension());
        QFile::link(f->path(), dir.absoluteFilePath(linkName));
    }
    */
}

void EpisodeList::readAsElement(nw::JsonReader &jr) {
    jr.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jr.enterNextElement(i); ++i) {
        Episode* episode = new Episode(&jr, this);
        addEpisode(episode);
    }
}

void EpisodeList::writeAsElement(nw::JsonWriter &jw) {
    jw.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        Episode* episode = episodes[i];
        episode->describe(&jw);
    }
}

void EpisodeList::writeDetailed(nw::JsonWriter &jw, const QStringList& releaseGroupPreference) {
    jw.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        Episode* episode = episodes[i];
        episode->writeDetailed(jw, releaseGroupPreference);
    }
}

void EpisodeList::addMovieFile(const VideoFile* movieFile) {
    if (movieFile->path.isEmpty() || NULL != getEpisodeForPath(movieFile->path)) {
        delete movieFile;
        return;
    }

    Episode* ep = getEpisodeForNumber(movieFile->numericEpisodeNumber());
    if (NULL != ep) {
        ep->addPath(movieFile);
        return;
    }

    addEpisode(new Episode(movieFile));
}

void EpisodeList::addEpisode(Episode *episode) {
    if (this->getEpisodeForNumber(episode->getEpisodeNumber())) {
        delete episode;
        return; // TODO merge paths
    }
    this->episodes.push_back(episode);
    connect(episodes.back(), SIGNAL(watchedChanged(bool,bool)), this, SLOT(watchedChanged(bool,bool)));
}

Episode *EpisodeList::getEpisodeForNumber(float number) {
    if (number < 0) {
        return NULL; // specials don't have a number
    }

    foreach (Episode* ep, episodes) {
        if (ep->getEpisodeNumber() == number) {
            return ep;
        }
    }
    return NULL;
}

int EpisodeList::numberOfEpisodes() const
{
    int count = 0;
    for (int i=0; i < episodes.length(); ++i) {
        if (!episodes.at(i)->isSpecial()) {
            ++count;
        }
    }
    return count;
}

int EpisodeList::numberOfWatchedEpisodes() const
{
    int count = 0;
    for (int i=0; i < episodes.length(); ++i) {
        if (!episodes.at(i)->isSpecial() && episodes.at(i)->getWatched()) {
            ++count;
        }
    }
    return count;
}


float EpisodeList::highestDownloadedEpisodeNumber() const
{
    float highest = -1;
    for (int i=0; i < episodes.length(); ++i) {
        float num = episodes.at(i)->getEpisodeNumber();
        highest = num > highest ? num : highest;
    }
    return highest;
}

QStringList EpisodeList::releaseGroups() const {
    QStringList groups;
    foreach (const Episode* ep, episodes) {
        QStringList epGroups = ep->releaseGroups();
        foreach (QString group, epGroups) {
            if (!groups.contains(group)) {
                groups.push_back(group);
            }
        }
    }
    return groups;
}

float EpisodeList::highestWatchedEpisodeNumber(int min) const
{
    float highest = min;
    for (int i=0; i < episodes.length(); ++i) {
        if (episodes.at(i)->getWatched()) {
            float num = episodes.at(i)->getEpisodeNumber();
            highest = num > highest ? num : highest;
        }
    }
    return highest;
}

QString EpisodeList::mostDownloadedReleaseGroup() const {
    QMap<QString, int> scores;
    for (int i=0; i < episodes.length(); ++i) {
        QStringList groups = episodes.at(i)->releaseGroups();
        foreach (QString group, groups) {
            scores[group]++;
        }
    }
    std::pair<QString, int> highest;
    for (QMap<QString, int>::iterator it=scores.begin(); it != scores.end(); ++it) {
        int num = it.value();
        if (num > highest.second) {
            highest.second = num;
            highest.first = it.key();
        }
    }
    return highest.first;
}

bool epNumLess(const Episode* a, const Episode* b) {
    return a->getEpisodeNumber() < b->getEpisodeNumber();
}

Episode* EpisodeList::getEpisodeForPath(const QString& path) {
    for (int i=0; i < episodes.length(); ++i) {
        Episode* f = episodes[i];
        const VideoFile* mf = f->getMovieFileForPath(path);
        if (mf) {
            return f;
        }
    }
    return NULL;
}

void EpisodeList::setMinimalWatched(int number) {
    foreach (Episode* ep, episodes) {
        float epNum = ep->getEpisodeNumber();
        // ignore 11.5 style special episodes
        if (epNum <= 0 || 0 != epNum - (int)epNum) {
            continue;
        }
        if (epNum <= number) {
            ep->setWatched(true);
        }
    }
}

void EpisodeList::setMaximalWatched(int number) {
    foreach (Episode* ep, episodes) {
        float epNum = ep->getEpisodeNumber();
        // ignore 11.5 style special episodes
        if (epNum <= 0 || 0 != epNum - (int)epNum) {
            continue;
        }
        if (epNum <= number) {
            ep->setWatched(true);
        } else {
            ep->setWatched(false);
        }
    }
}

void EpisodeList::setWatched(int number) {
    foreach (Episode* ep, episodes) {
        if (ep->isSpecial()) {
            continue;
        }

        int epNum = ep->getEpisodeNumber();
        if (epNum <= number) {
            ep->setWatched(true);
        } else {
            ep->setWatched(false);
        }
    }
}

void EpisodeList::watchedChanged(bool oldValue, bool newValue) {
    int count = numberOfWatchedEpisodes();
    if (!oldValue && newValue) {
        emit watchCountChanged(count-1, count);
    } else if (oldValue && !newValue) {
        emit watchCountChanged(count+1, count);
    } else {
        emit watchCountChanged(count, count);
    }
}


void EpisodeList::beforeWatchedChanged(bool newValue, bool oldValue) {
    Episode* ep = dynamic_cast<Episode*>(sender());
    if (!ep) throw ("fuck up ep casting" + std::string(__func__));

    if (oldValue != newValue && !ep->isSpecial()) {
        int count = numberOfWatchedEpisodes();
        int diff = oldValue ? -1 : +1;
        emit beforeWatchCountChanged(count+diff, count);
    }
}

QList<const VideoFile*> EpisodeList::missingFiles() const {
    QList<const VideoFile*> missing;
    foreach (Episode* ep, episodes) {
        missing << ep->missingFiles();
    }
    return missing;
}

bool EpisodeList::removeFile(QString filepath) {
    foreach (Episode* const& e, episodes) {
        bool success = e->removeFile(filepath);
        if (success) {
            return success;
        }
    }
    return false;
}
