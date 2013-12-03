#include "episodelist.h"
#include <QDebug>
#include "nwutils.h"

EpisodeList::EpisodeList(QString name, QObject* parent) : QObject(parent) {
    mName = name;
}

EpisodeList::~EpisodeList() {
}

void EpisodeList::exportXbmcLinks(QDir dir) {
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
    NwUtils::describe(jr, "name", mName);
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

void EpisodeList::writeDetailed(nw::JsonWriter &jw) {
    NwUtils::describe(jw, "name", mName);
    jw.describeArray("episodes", "episode", episodes.length());
    for (int i=0; jw.enterNextElement(i); ++i) {
        Episode* episode = episodes[i];
        episode->writeDetailed(jw);
    }
}

void EpisodeList::addMovieFile(const MovieFile* movieFile) {
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

QString EpisodeList::name() const {
    return mName;
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

float EpisodeList::highestWatchedEpisodeNumber() const
{
    float highest = -1;
    for (int i=0; i < episodes.length(); ++i) {
        if (episodes.at(i)->getWatched()) {
            float num = episodes.at(i)->getEpisodeNumber();
            highest = num > highest ? num : highest;
        }
    }
    return highest;
}

QString EpisodeList::mostDownloadedReleaseGroup() const {
    QMap<QString, int> groups;
    for (int i=0; i < episodes.length(); ++i) {
        //QString group = episodes.at(i)->releaseGroups();
        //groups[group]++;
    }
    std::pair<QString, int> highest;
    for (QMap<QString, int>::iterator it=groups.begin(); it != groups.end(); ++it) {
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

QString EpisodeList::favouriteReleaseGroup() const {
    /*
    QList<Episode* > episodesByNumber = episodes;
    qSort(episodesByNumber.begin(), episodesByNumber.end(), epNumLess);

    if (episodesByNumber.at(0)->releaseGroup().compare(
            episodesByNumber.at(1)->releaseGroup(),
            Qt::CaseInsensitive
        )
    ) {
        return episodesByNumber.at(0)->releaseGroup();
    }
    */
    return mostDownloadedReleaseGroup();
}

Episode* EpisodeList::getEpisodeForPath(const QString& path) {
    for (int i=0; i < episodes.length(); ++i) {
        Episode* f = episodes[i];
        const MovieFile* mf = f->getMovieFileForPath(path);
        if (mf) {
            return f;
        }
    }
    return NULL;
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
