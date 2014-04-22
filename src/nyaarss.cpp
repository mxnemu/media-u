#include "nyaarss.h"
#include "QTextDocument"

namespace NyaaRss {

Feed::Feed(QString url,  const RssConfig& rssConfig, TvShow* tvShow) :
    TorrentRss::Feed(url,  rssConfig, tvShow)
{
}

TorrentRss::FeedResult* Feed::createFeedResult(const RssConfig& rssConfig) {
    return new FeedResult(rssConfig);
}

FeedResult::FeedResult(const RssConfig& rssConfig) :
    TorrentRss::FeedResult(rssConfig)
{
}

void FeedResult::parse(CurlResult& result) {
    nw::XmlReader xr(result.data);
    xr.push("rss");
    xr.push("channel");
    xr.describeArray("", "item", -1);
    for (int i=0; xr.enterNextElement(i); ++i) {
        Entry* entry = new Entry();
        entry->parse(&xr);
        this->entires.push_back(entry);
    }
    xr.close();
}



void Entry::parse(nw::Describer* de) {
    QString dateString;
    QString typeStr;
    NwUtils::describe(*de, "title", this->name);
    NwUtils::describe(*de, "link", this->url);
    NwUtils::describe(*de, "pubDate", dateString);
    NwUtils::describe(*de, "category", typeStr);

    this->url = QUrl(this->url).toString(QUrl::FormattingOptions(QUrl::PrettyDecoded));

    // TODO remove decode workaround and do it properly
    QTextDocument text;
    text.setHtml(this->url);
    this->url = text.toPlainText();

    if (typeStr == Entry::rawAnimeStr) type = rawAnime;
    else if (typeStr == Entry::nonEnglishAnimeStr) type = nonEnglishAnime;
    else if (typeStr == Entry::englishAnimeStr) type = englishAnime;
    else type = other;

    this->date = parseDate(dateString);
}


bool Entry::isCandidateForAutoDownload(QString query, int episode, QString subgroup, const RssConfig& rssConfig) {
    const VideoFile parsed = this->name;
    if (0 == parsed.showName.compare(query, Qt::CaseInsensitive) &&
        parsed.numericEpisodeNumber() ==  episode &&
        (rssConfig.requireFavouriteReleaseGroup ? parsed.releaseGroup.contains(subgroup) : true) &&
        (
            (this->type == rawAnime && rssConfig.includeRaw) ||
            (this->type == englishAnime && rssConfig.includeEnglish)
        )) {
        return true;
    }
    return false;
}

 const QString Entry::rawAnimeStr = "Raw Anime";
 const QString Entry::nonEnglishAnimeStr = "Non-English-translated Anime";
 const QString Entry::englishAnimeStr = "English-translated Anime";



// I could not find a way to solve this with the default format
QDateTime parseDate(QString string) {
    // example: Sat, 30 Nov 2013 16:41:27 +0000
    QRegExp regex("..., ([0-9]+) ([a-zA-Z]+) ([0-9]+ [0-9:]+)");
    int found = regex.indexIn(string);
    if (-1 != found) {
        static const QStringList months =
                QStringList() << "Jan" << "Feb" << "Mar" << "Apr" <<
                                 "May" << "Jun" << "Jul" << "Aug" <<
                                 "Sep" << "Oct" << "Nov" << "Dez";
        int month = months.indexOf(regex.cap(2));
        if (-1 == month) {
            return QDateTime();
        }
        month += 1;
        QString monthString = QString("%1").arg(month, 2, 10, QChar('0'));
        QString simplified = QString("%1 %2 %3").arg(regex.cap(1), monthString, regex.cap(3));
        return QDateTime::fromString(simplified, "dd MM yyyy hh:mm:ss");
    }
    return QDateTime();
}

Client::Client(TorrentClient& torrentClient, Library& library, const RssConfig& rssConfig, QObject* parent) :
    TorrentRss::Client(torrentClient, library, rssConfig, parent)
{
}

void Client::addFeed(TvShow* tvShow) {
    int nextEpisode = tvShow->episodeList().highestDownloadedEpisodeNumber() + 1;
    // TODO properly prefix the number (2, 3) otherwise nyaa won't find it
    QString number = QString("%1").arg(nextEpisode, 2, 10, QChar('0'));
    QString query = QString("%1 %2").arg(tvShow->name(), number);
    QString url = QString("http://www.nyaa.se/?page=rss&term=%1").arg(query);
    TorrentRss::Client::addFeed(new Feed(url, rssConfig, tvShow));
}

} // < namespace
