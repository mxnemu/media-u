#include "gelbooruclient.h"
#include <QDebug>
#include "filedownloadthread.h"

namespace Gelbooru {

Client::Client(QString baseUrl, int limit, Rating ratingFilter) :
    WallpaperDownload::Client(baseUrl, limit, ratingFilter)
{
}

SearchResult Client::fetchPostsBlocking(QString tagName, int page) {
    if (tagName.isEmpty() || tagName.isNull()) {
        return SearchResult();
    }
    tagName.replace(' ', '_');

    CurlResult userData(this);
    CURL* handle = curlClient(tagName, userData, page);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for tagquery '" << tagName << "'' with this message:\n";
        userData.print();
    } else {
        return parseSearchResult(userData.data, limit);
    }
    return SearchResult();
}

Entry Client::parseEntry(nw::Describer *de) {
    int height = -1;
    int width = -1;
    int score = -1;

    Entry entry;
    NwUtils::describe(*de, "file_url", entry.fileUrl);
    NwUtils::describe(*de, "sample_url", entry.sampleUrl);
    NwUtils::describe(*de, "preview_url", entry.previewUrl);
    NwUtils::describe(*de, "rating", entry.rating);
    NwUtils::describe(*de, "id", entry.id);
    NwUtils::describe(*de, "tags", entry.tags, ' ');

    // optional TODO use it
    NwUtils::describe(*de, "height", height);
    NwUtils::describe(*de, "width", width);
    NwUtils::describe(*de, "score", score);
    return entry;
}

SearchResult Client::parseSearchResult(std::stringstream &ss, int limit)
{
    int count = 0;
    int offset = 0;

    SearchResult sr(limit);
    nw::XmlReader xr(ss);
    xr.describe("count", count);
    xr.describe("offset", offset);

    xr.describeArray("","post", 0);
    for (int i=0; xr.enterNextElement(i); ++i) {
        sr.entries.push_back(parseEntry(&xr));
    }
    return sr;
}

CURL *Client::curlClient(QString tag, CurlResult& userdata, const unsigned int page)
{
    QString pageStr = QString::number(page);
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, QString("%1/index.php?page=dapi&s=post&q=index&pid=%2&tags=%3").arg(baseUrl, pageStr, tag).toLocal8Bit().data());
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

};


