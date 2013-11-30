#include "torrentrss.h"
#include <QDebug>

namespace TorrentRss {

Client::Client(QObject *parent) :
    QObject(parent)
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

void Client::addFeed(Feed* feed) {
    this->feeds.push_back(feed);
}

Thread::Thread(QObject* parent) :
    QThread(parent)
{
    this->toldToStop = false;
    this->refetchInterval = 5000 * 60;
    this->sleeped = this->refetchInterval;
    this->sleepStep = 1000;
}

void Thread::run() {
    while (!this->toldToStop) {
        if (this->sleeped >= this->refetchInterval) {
            this->client.refetch();
            this->sleeped = 0;
        }
        msleep(this->sleepStep);
        this->sleeped = this->sleepStep;
    }
}

Feed::Feed() : result(NULL) {
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
}

FeedResult::~FeedResult() {
    foreach (Entry* entry, this->entires) {
        delete entry;
    }
}

} // namespace
