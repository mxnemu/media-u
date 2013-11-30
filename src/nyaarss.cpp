#include "nyaarss.h"
#include "nwutils.h"

namespace NyaaRss {

TorrentRss::FeedResult* Feed::createFeedResult() {
    return new FeedResult();
}

void FeedResult::parse(CurlResult& result) {
    nw::XmlReader xr(result.data);
    xr.push("rss");
    xr.push("channel");
    xr.describeArray("", "item", -1);
    for (int i=0; xr.enterNextElement(i); ++i) {
        TorrentRss::Entry* entry = new TorrentRss::Entry();
        QString dateString;
        NwUtils::describe(xr, "title", entry->name);
        NwUtils::describe(xr, "link", entry->url);
        NwUtils::describe(xr, "pubDate", dateString);
        entry->date = parseDate(dateString);
        this->entires.push_back(entry);
    }
    xr.close();
}

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

}
