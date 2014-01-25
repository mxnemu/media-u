#ifndef NYAARSS_H
#define NYAARSS_H

#include "torrentrss.h"
#include "nwutils.h"

namespace NyaaRss {

class Entry : public TorrentRss::Entry {
public:
    void parse(nw::Describer* de);

    enum Type {
        notSet,
        invalid,
        other,
        rawAnime,        //"Raw Anime"
        nonEnglishAnime, //"Non-English-translated Anime"
        englishAnime    //"English-translated Anime"

    };

    static const QString rawAnimeStr;
    static const QString nonEnglishAnimeStr;
    static const QString englishAnimeStr;

    bool isCandidateForAutoDownload(QString query, int episode, QString subgroup, const RssConfig& rssConfig);
protected:
    Type type;
};

class FeedResult : public TorrentRss::FeedResult {
public:
    FeedResult(const RssConfig &rssConfig);
private:
    void parse(CurlResult &result);
};

class Feed : public TorrentRss::Feed {
public:
    Feed(QString url, const RssConfig& rssConfig, TvShow* tvShow = NULL);
    TorrentRss::FeedResult* createFeedResult(const RssConfig& rssConfig);
};

class Client : public TorrentRss::Client {
public:
    Client(TorrentClient &torrentClient, Library &library, const RssConfig& rssConfig, QObject *parent = NULL);

    virtual void addFeed(TvShow* show);
};

    QDateTime parseDate(QString dateString);

}

#endif // NYAARSS_H
