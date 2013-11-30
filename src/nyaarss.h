#ifndef NYAARSS_H
#define NYAARSS_H

#include "torrentrss.h"

namespace NyaaRss {

class FeedResult : public TorrentRss::FeedResult {
    void parse(CurlResult &result);
};

class Feed : public TorrentRss::Feed {
public:
    TorrentRss::FeedResult* createFeedResult();
};

    QDateTime parseDate(QString dateString);

}

#endif // NYAARSS_H
