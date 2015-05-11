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

Client::Client(OnlineCredentials& credentials, QObject *parent) :
    OnlineTvShowDatabase::Client(credentials, parent)
{
}

OnlineTvShowDatabase::SearchResult* Client::search(QString anime) {
    QString name = anime;
    if (name.isEmpty() || name.isNull()) {
        return NULL;
    }

    QString url = "http://myanimelist.net/api/anime/search.xml?q=";
    url.append(name.replace(' ', '+').remove('~'));

    CurlResult userData(this);
    CURL* handle = credentials.curlClientNoLock(url.toLocal8Bit().data(), userData);
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

const QString Client::IDENTIFIER_KEY = "mal";
const QString Client::identifierKey() const {
    return IDENTIFIER_KEY;
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

int Entry::getRemoteId() const {
    return id.toInt();
}

void Entry::updateSynopsis(TvShow& show) const {
    show.setSynopsis(synopsis);
}

void Entry::updateTitle(TvShow&) const {
    //show.setName();
//    show.setLongTitle(title);
}

void Entry::updateRemoteId(TvShow& show) const {
    show.setRemoteId(Client::IDENTIFIER_KEY, id.toInt());
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
    OnlineTvShowDatabase::SearchResult(query)
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
        entries.back().calculateQuerySimiliarity(searchedQuery);
        hasEntries = true;
    }
    if (!hasEntries) {
        qDebug() << "no results for mal search >" << searchedQuery;
    }
    xr.close();
}

OnlineTvShowDatabase::Entry* SearchResult::bestEntry() {
    std::pair<int, Entry*> best(-1, NULL);
    for (int i=0; i < entries.length(); ++i) {
        Entry* entry = &entries[i];

        int score = Utils::querySimiliarity(this->searchedQuery, entry->title);
        /*
        foreach (const QString& name, entry->englishTitles) {
            int s = Utils::querySimiliarity(this->query, name);
            score = score >= s ? score : s;
        }
        */
        foreach (const QString& name, entry->synonyms) {
            int s = Utils::querySimiliarity(this->searchedQuery, name);
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

} // namespace
