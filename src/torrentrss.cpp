#include "torrentrss.h"
#include <QDebug>

namespace TorrentRss {

Client::Client(TorrentClient& torrentClient, Library& library, QObject *parent) :
    QObject(parent),
    library(library),
    torrentClient(torrentClient)
{
}

Client::~Client() {
    foreach (Feed* feed, this->feeds) {
        delete feed;
    }
}

void Client::refetch() {
    foreach (Feed* feed, this->feeds) {
        feed->fetch();
    }
}

void Client::connectLibrary() {
    connect(&library, SIGNAL(searchFinished()), this, SLOT(addFeedsForWaitingShows()));
}

void Client::addFeed(Feed* feed) {
    foreach (Feed* f, feeds) {
        if (f->getTvShow() == feed->getTvShow()) {
            delete feed;
            return;
        }
    }

    this->feeds.push_back(feed);
    connect(feed, SIGNAL(foundCandidateForAutoDownload(Entry)), this, SLOT(autoDownloadEntry(Entry)));
    feed->fetch();
}

void Client::addFeedsForWaitingShows() {
    QList<TvShow *> shows = library.filter().statusWaitingForNewEpisodes();
    foreach (TvShow* show, shows) {
        addFeed(show);
    }

    QList<TvShow *> airing = library.filter().airing();
    foreach (TvShow* show, airing) {
        if (!show->completed) {
            addFeed(show);
        }
    }
}

void Client::autoDownloadEntry(Entry entry) {
    Feed* feed = dynamic_cast<Feed*>(sender());
    this->removeFeed(feed);

    torrentClient.addTorrent(entry.url);
}

void Client::removeFeed(Feed* feed) {
    feeds.removeOne(feed);
    if (feed) {
        feed->deleteLater();
    }
}

Thread::Thread(Client& client, QObject* parent) :
    QThread(parent),
    client(client)
{
    this->toldToStop = false;
    this->refetchInterval = 15000 * 60;
    this->sleeped = this->refetchInterval;
    this->sleepStep = 1000;
}

Thread::~Thread() {
    toldToStop = true;
}

void Thread::run() {
    // TODO use wait condition and store refetch time for each feed separated
    while (!this->toldToStop) {
        if (this->sleeped >= this->refetchInterval) {
            this->client.refetch();
            this->sleeped = 0;
        }
        msleep(this->sleepStep);
        this->sleeped += this->sleepStep;
    }
}

void Client::tvShowChangedStatus(TvShow* show, TvShow::WatchStatus newStatus, TvShow::WatchStatus oldStatus) {
    if (newStatus == TvShow::waitingForNewEpisodes) {
        this->addFeed(show);
    }
}

Feed::Feed(QString url, TvShow* tvShow) :
    QObject(NULL),
    result(NULL),
    url(url),
    tvShow(tvShow)
{
}

Feed::~Feed() {
}

void Feed::fetch() {
    CurlResult userdata;
    CURL* handle = defaultCurlClient(this->url, userdata);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error) {
        qDebug() << "failed to fetch rss url" << this->url;
        return;
    }
    FeedResult* newResult = this->createFeedResult();
    newResult->parse(userdata);
    this->setResult(newResult);
}

TvShow*Feed::getTvShow() {
    return this->tvShow;
}

Entry*Feed::candidateForAutoDownload() {
    if (!this->getTvShow()) {
        return NULL;
    }
    QString name = tvShow->name();
    QString releaseGroup = tvShow->episodeList().favouriteReleaseGroup();
    int nextEpisode = tvShow->episodeList().highestDownloadedEpisodeNumber() + 1;
    foreach(Entry* entry, result->entires) {
        if (entry->isCandidateForAutoDownload(name, nextEpisode, releaseGroup)) {
            return entry;
        }
    }
    return NULL;
}

CURL* Feed::defaultCurlClient(QString url, CurlResult& userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

void Feed::setResult(FeedResult* result) {
    if (this->result) {
        delete this->result;
    }
    this->result = result;
    Entry* torrent = candidateForAutoDownload();
    if (torrent) {
        emit foundCandidateForAutoDownload(Entry(*torrent));
    }
}


FeedResult::~FeedResult() {
    foreach (Entry* entry, this->entires) {
        delete entry;
    }
}

} // namespace
