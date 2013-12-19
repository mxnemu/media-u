#include "malclient.h"
#include <qhttpserver.h>
#include <qhttpconnection.h>
#include <QTcpSocket>
#include <QDebug>
#include <iostream>
#include <QFile>
#include "nwutils.h"
#include "utils.h"

namespace Mal {

Client::Client(QObject *parent) :
    OnlineTvShowDatabase::Client(parent),
    userAgent("nemu-malapiclient"),
    activeThread(NULL)
{
    mHasValidCredentials = false;
}

void Client::init(QString configFilePath) {
    if (QFile(configFilePath).exists()) {
        std::string user, password;

        nw::JsonReader jr(configFilePath.toStdString());
        jr.describe("user", user);
        jr.describe("password", password);
        NwUtils::describe(jr, "userAgent", userAgent);
        jr.close();

        if (user.length() > 0 && password.length() > 0) {
            this->setCredentials(QString(user.data()), QString(password.data()));
        }
    }
}

void Client::fetchShows(QList<TvShow*> &showList, QDir libraryDir) {
    if (activeThread) {
        return;
    }
    activeThread = new Thread(*this, showList, libraryDir, this);
    activeThread->start(QThread::LowPriority);
    qDebug() << "started mal fetchThread";

    connect(activeThread, SIGNAL(finished()),
            this, SLOT(fetchThreadFinished()));
}

void Client::fetchThreadFinished() {
    if (activeThread == sender()) {
        delete activeThread;
        this->activeThread = NULL;
    } else {
        throw "fetchThreadFinished() signal received from unknown sender()";
    }
    emit fetchingFinished();
}

OnlineTvShowDatabase::SearchResult* Client::search(QString anime) {
    QString name = anime;
    if (name.isEmpty() || name.isNull()) {
        return NULL;
    }

    QString url = "http://myanimelist.net/api/anime/search.xml?q=";
    url.append(name.replace(' ', '+'));

    CurlResult userData(this);
    CURL* handle = curlClient(url.toLocal8Bit().data(), userData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for query '" << url << "'' with this message:\n";
        userData.print();
    } else {
        return new SearchResult(userData, name);
    }
    return NULL;
}

const OnlineTvShowDatabase::Entry*Client::bestResult(const OnlineTvShowDatabase::SearchResult& result) const {
    return ((SearchResult&)result).bestResult(); // not a good cast use pointers
}



void Client::setCredentials(const QString name, const QString password) {
    this->username = name;
    this->password = password;
}

bool Client::verifyCredentials() {
    if (username.length() <= 0 || password.length() <= 0) {
        return false;
    }

    CurlResult userData(this);
    CURL* handle = curlClient("http://myanimelist.net/api/account/verify_credentials.xml", userData);
    CURLcode error = curl_easy_perform(handle);
    if (error) {
        qDebug() << "received error " << error << " with this message:\n";
        userData.print();
    } else {
        if (userData.data.str() == "Invalid credentials") {
            mHasValidCredentials = false;
        } else {
            mHasValidCredentials = true;
        }
    }

    qDebug() << "mal connection is " << mHasValidCredentials;
    curl_easy_cleanup(handle);
    return mHasValidCredentials;
}

CURL* Client::curlClient(const char* url, CurlResult& userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, userAgent.toLatin1().data());
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

bool Client::hasValidCredentials() const {
    return mHasValidCredentials;
}



///////////////////////////////////////////////////////////////////
//
// MAL Thread
//
//////////////////////////////////////////////////////////////////

Thread::Thread(Client &client, QList<TvShow*> &shows, QDir libraryDir, QObject *parent) :
    OnlineTvShowDatabase::Thread(client, shows, libraryDir, parent)
{

}

void Thread::run() {
    QTime loginTimer;
    loginTimer.start();
    Mal::Client& malClient = (Mal::Client&)this->client; // <- if I fuck this up debugging hell is here
    if (!malClient.hasValidCredentials() && !malClient.verifyCredentials()) {
        qDebug() << "can't fetch data, no valid login credentials";
        return;
    }
    int loginSleep = 3000 - loginTimer.elapsed();
    if (loginSleep > 0) {
        msleep(loginSleep);
    }
    // TODO reuse code from OnlineTvShowDatabase
    for (QList<TvShow*>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        TvShow& show = *(it.i->t());
        if (show.getRemoteId() == -1 ||
            show.getTotalEpisodes() == 0 ||
            (show.isAiring() && show.getStatus() == TvShow::completed)) {
            QTime timer;
            timer.start();

            SearchResult* searchResult = dynamic_cast<SearchResult*>(malClient.search(show.name()));
            if (!searchResult) {
                continue;
            }
            const Entry* entry = dynamic_cast<const Entry*>(malClient.bestResult(*searchResult));
            if (!entry) {
                continue;
            }
            entry->updateShow(show, libraryDir);

            int sleepTime = 3000 - timer.elapsed();
            if (sleepTime > 0) {
                msleep(sleepTime);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////
//
// MAL ENTRY
//
//////////////////////////////////////////////////////////////////

void Entry::calculateQuerySimiliarity(const QString query) {
    int titleResult = Utils::querySimiliarity(query, title);
    int englishTitleResult = Utils::querySimiliarity(query, englishTitle);

    int bestResult = titleResult > englishTitleResult ? titleResult : englishTitleResult;

    for (int i=0; i < synonyms.length(); ++i) {
        const QString& synonym = synonyms.at(i);
        int result = Utils::querySimiliarity(query, synonym);
        if (result > bestResult) {
            bestResult = result;
        }
    }
    querySimiliarityScore = bestResult;
}

void Entry::updateSynopsis(TvShow& show) const {
    show.setSynopsis(synopsis);
}

void Entry::updateTitle(TvShow&) const {
    //show.setName();
//    show.setLongTitle(title);
}

void Entry::updateRemoteId(TvShow& show) const {
    show.setRemoteId(id.toInt());
}

void Entry::updateRelations(TvShow& ) const {

}

void Entry::updateAiringDates(TvShow& show) const {
    show.setTotalEpisodes(episodes);
    show.setShowType(type);
    show.setAiringStatus(status);
    show.setStartDate(QDate::fromString(startDate, Entry::dateFormat));
    show.setEndDate(QDate::fromString(endDate, Entry::dateFormat));
}

void Entry::updateSynonyms(TvShow& show) const {
    show.setSynonyms(synonyms);
}

void Entry::updateImage(TvShow& show, QDir libraryDir) const {
    show.downloadImage(image, libraryDir);
}

Entry::Entry(nw::XmlReader& reader) {
    parse(reader);
    querySimiliarityScore = 0;
}

void Entry::parse(nw::XmlReader &xr) {
    NwUtils::describe(xr, "id", id);
    NwUtils::describe(xr, "title", title);
    NwUtils::describe(xr, "englishTitle", englishTitle);
    this->parseSynonyms(xr);
    NwUtils::describe(xr, "episodes", episodes);
    NwUtils::describe(xr, "type", type);
    NwUtils::describe(xr, "status", status);
    NwUtils::describe(xr, "start_date", startDate);
    NwUtils::describe(xr, "end_date", endDate);
    NwUtils::describe(xr, "synopsis", synopsis);
    NwUtils::describe(xr, "image", image);

    title = QUrl::fromPercentEncoding(title.toLatin1());
    image = QUrl::fromPercentEncoding(image.toLatin1());
    synopsis = QUrl::fromPercentEncoding(synopsis.toLatin1());
}

void Entry::parseSynonyms(nw::XmlReader &reader) {
    QString synonyms;
    NwUtils::describe(reader, "synonyms", synonyms);
    synonyms = QUrl::fromPercentEncoding(synonyms.toLatin1());
    this->synonyms = synonyms.split(QRegExp("; "));
}

QString Entry::dateFormat = "yyyy-MM-dd";


///////////////////////////////////////////////////////////////////
//
// MAL Search result
//
//////////////////////////////////////////////////////////////////

SearchResult::SearchResult(CurlResult &result, QString query) :
    query(query)
{
    parse(result);
}

void SearchResult::parse(CurlResult &result) {
    std::cout.flush();
    nw::XmlReader xr(result.data);
    xr.push("anime");
    xr.describeArray("", "entry", 0);
    bool hasEntries = false;
    for (int i=0; xr.enterNextElement(i); ++i) {
        entries.append(Entry(xr));
        entries.back().calculateQuerySimiliarity(query);
        hasEntries = true;
    }
    if (!hasEntries) {
        qDebug() << "no results for mal search >" << query;
    }
    xr.close();
}

const Entry* SearchResult::bestResult() const {
    std::pair<int, const Entry*> best(-1, NULL);
    for (int i=0; i < entries.length(); ++i) {
        const Entry* entry = &entries.at(i);

        int score = Utils::querySimiliarity(this->query, entry->title);
        /*
        foreach (const QString& name, entry->englishTitles) {
            int s = Utils::querySimiliarity(this->query, name);
            score = score >= s ? score : s;
        }
        */
        foreach (const QString& name, entry->synonyms) {
            int s = Utils::querySimiliarity(this->query, name);
            score = score >= s ? score : s;
        }
        /*
        foreach (const QString& name, entry->japaneseTitles) {
            int s = Utils::querySimiliarity(this->query, name);
            score = score >= s ? score : s;
        }
        */
        if (score > best.first) {
            best.first = score;
            best.second = entry;
        }
    }
    return best.second;
}

void SearchResult::updateShowFromBestEntry(TvShow &show, QDir libraryDir) const {
    const Entry* entry = bestResult();

    if (entry) {
        entry->updateShow(show, libraryDir);
        qDebug() << "updated " << show.getShowType() << show.name();
    }
}


AnimeUpdateData::AnimeUpdateData(TvShow *show) {
    this->episode = show->episodeList().highestWatchedEpisodeNumber();
    this->status = calculateWatchStatus(this->episode, show->getTotalEpisodes());
    this->downloaded_episodes = show->episodeList().numberOfEpisodes();

    score = -1;
    storage_type = -1; // int (will be updated to accomodate strings soon) // yeah sure soon...
    storage_value = -1; // wat
    times_rewatched = -1;
    rewatch_value = -1; // 0 - 10 ? dont know didn't check
    priority = -1; // 0 - 10 ? dont know didn't check
    enable_discussion = 0; // int. 1=enable, 0=disable
    enable_rewatching = -1; // int. 1=enable, 0=disable
    fansub_group = show->episodeList().mostDownloadedReleaseGroup();
    QStringList tags; // string. tags separated by commas
}

UpdateWatchStatus AnimeUpdateData::calculateWatchStatus(int episodesWatched, int total) {
    if (episodesWatched == 0) {
        return plantowatch;
    } else if (episodesWatched == total) {
        return completed;
    } else if (episodesWatched > 0) {
        return watching;
    }
    return onhold;
}

Thread* Client::getActiveThread() const
{
    return activeThread;
}

} // namespace
