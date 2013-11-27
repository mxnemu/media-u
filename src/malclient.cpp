#include "malclient.h"
#include <qhttpserver.h>
#include <qhttpconnection.h>
#include <QTcpSocket>
#include <QDebug>
#include <iostream>
#include <QFile>
#include "nwutils.h"
#include "utils.h"

MalClient::MalClient(QObject *parent) :
    QObject(parent),
    userAgent("nemu-malapiclient"),
    activeThread(NULL)
{
    mHasValidCredentials = false;
}

void MalClient::init(QString configFilePath) {
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

void MalClient::fetchShows(QList<TvShow*> &showList, QDir libraryDir) {
    if (activeThread) {
        return;
    }
    activeThread = new MalClientThread(*this, showList, libraryDir, this);
    //connect(this, SIGNAL(destroyed()), activeThread, SLOT(terminate()));
    activeThread->start(QThread::LowPriority);
    qDebug() << "started mal fetchThread";

    connect(activeThread, SIGNAL(finished()),
            this, SLOT(fetchThreadFinished()));
}

void MalClient::fetchThreadFinished() {
    if (activeThread == sender()) {
        delete activeThread;
        this->activeThread = NULL;
    } else {
        throw "fetchThreadFinished() signal received from unknown sender()";
    }
    emit fetchingFinished();
}

void MalClient::fetchShowBlocking(TvShow& show, QDir libraryDir) {
    QString name = show.name();
    if (name.isEmpty() || name.isNull()) {
        return;
    }

    QString url = "http://myanimelist.net/api/anime/search.xml?q=";
    //url.append(QUrl::toPercentEncoding(name));
    url.append(name);

    CurlResult userData(this);
    CURL* handle = curlClient(url.toLocal8Bit().data(), userData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for query '" << url << "'' with this message:\n";
        userData.print();
    } else {
        MalSearchResult result(userData, name);
        result.updateShowFromBestEntry(show, libraryDir);
    }
}



void MalClient::setCredentials(const QString name, const QString password) {
    this->username = name;
    this->password = password;
}

bool MalClient::verifyCredentials() {
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

CURL* MalClient::curlClient(const char* url, CurlResult& userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, userAgent.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

/*
CurlXmlResult MalClient::curlPerform(const char* url) {
    CurlXmlResult userData(this);
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, MalClient::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, userData);
    userData.curlError = curl_easy_perform(handle);
    return userData;
}
*/

bool MalClient::hasValidCredentials() const {
    return mHasValidCredentials;
}



///////////////////////////////////////////////////////////////////
//
// MAL Thread
//
//////////////////////////////////////////////////////////////////

MalClientThread::MalClientThread(MalClient &client, QList<TvShow*> &shows, QDir libraryDir, QObject *parent) :
    QThread(parent),
    malClient(client),
    tvShows(shows),
    libraryDir(libraryDir)
{

}

void MalClientThread::run() {
    QTime loginTimer;
    loginTimer.start();
    if (!malClient.hasValidCredentials() && !malClient.verifyCredentials()) {
        qDebug() << "can't fetch data, no valid login credentials";
        return;
    }
    int loginSleep = 3000 - loginTimer.elapsed();
    if (loginSleep > 0) {
        msleep(loginSleep);
    }

    for (QList<TvShow*>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        TvShow& show = *(it.i->t());
        if (show.getRemoteId() == -1) {
            QTime timer;
            timer.start();

            malClient.fetchShowBlocking(show, libraryDir);

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

void MalEntry::calculateQuerySimiliarity(const QString query) {
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

MalEntry::MalEntry(nw::XmlReader& reader) {
    parse(reader);
    querySimiliarityScore = 0;
}

void MalEntry::parse(nw::XmlReader &xr) {
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



void MalEntry::parseSynonyms(nw::XmlReader &reader) {
    QString synonyms;
    NwUtils::describe(reader, "synonyms", synonyms);
    synonyms = QUrl::fromPercentEncoding(synonyms.toLatin1());
    this->synonyms = synonyms.split(QRegExp("; "));
}

QString MalEntry::dateFormat = "yyyy-MM-dd";

void MalEntry::updateShowFromEntry(TvShow &show, QDir libraryDir) const {
    //show.setName();
//    show.setLongTitle(title);
    show.setTotalEpisodes(episodes);
    show.setShowType(type);
    show.setAiringStatus(status);
    show.setStartDate(QDate::fromString(startDate, MalEntry::dateFormat));
    show.setEndDate(QDate::fromString(endDate, MalEntry::dateFormat));
    show.setSynopsis(synopsis);
    show.setRemoteId(id.toInt());

    show.downloadImage(image, libraryDir);
}

///////////////////////////////////////////////////////////////////
//
// MAL Search result
//
//////////////////////////////////////////////////////////////////

MalSearchResult::MalSearchResult(CurlResult &result, QString query) :
    query(query)
{
    parse(result);
}

void MalSearchResult::parse(CurlResult &result) {
    std::cout.flush();
    nw::XmlReader xr(result.data);
    xr.push("anime");
    xr.describeArray("", "entry", 0);
    for (int i=0; xr.enterNextElement(i); ++i) {
        entries.append(MalEntry(xr));
        entries.back().calculateQuerySimiliarity(query);
    }
    xr.close();
}

const MalEntry* MalSearchResult::bestResult() const {
    std::pair<int, const MalEntry*> best(-1, NULL);
    for (int i=0; i < entries.length(); ++i) {
        const MalEntry* entry = &entries.at(i);

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

void MalSearchResult::updateShowFromBestEntry(TvShow &show, QDir libraryDir) const {
    const MalEntry* entry = bestResult();

    if (entry) {
        entry->updateShowFromEntry(show, libraryDir);
        qDebug() << "updated " << show.getShowType() << show.name();
    }
}


MalUpdaterAnimeData::MalUpdaterAnimeData(TvShow *show) {
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

MalUpdaterWatchStatus MalUpdaterAnimeData::calculateWatchStatus(int episodesWatched, int total) {
    if (episodesWatched == 0) {
        return plantowatch;
    } else if (episodesWatched == total) {
        return completed;
    } else if (episodesWatched > 0) {
        return watching;
    }
    return onhold;
}
