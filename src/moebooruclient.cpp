#include "moebooruclient.h"
#include "filedownloadthread.h"

namespace Moebooru {

Client::Client(QString baseUrl, int limit, Rating ratingFilter) :
    WallpaperDownload::Client(baseUrl, limit, ratingFilter)
{
}

CURL *Client::curlClient(QString tag, CurlResult& userdata, const unsigned int page) {
    QString pageStr = QString::number(page);
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, QString("%1/post.json?page=%2&tags=%3").arg(baseUrl, pageStr, tag).toLocal8Bit().data());
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

SearchResult Client::parseSearchResult(std::stringstream& ss, int limit) {
    SearchResult sr(limit);
    nw::JsonReader jr(ss);
    jr.describeArray("","", 0);
    for (int i=0; jr.enterNextElement(i); ++i) {
        sr.entries.push_back(parseEntry(&jr));
    }
    return sr;
}

Entry Client::parseEntry(nw::Describer *de) {
    Entry entry;
    NwUtils::describe(*de, "id", entry.id);
    NwUtils::describe(*de, "rating", entry.rating);
    NwUtils::describe(*de, "file_url", entry.fileUrl);
    NwUtils::describe(*de, "sample_url", entry.sampleUrl);
    NwUtils::describe(*de, "preview_url", entry.previewUrl);
    NwUtils::describe(*de, "tags", entry.tags, ' ');
    NwUtils::describe(*de, "width", entry.width);
    NwUtils::describe(*de, "height", entry.height);
    NwUtils::describe(*de, "score", entry.score);
    return entry;
}

};


