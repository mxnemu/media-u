#ifndef MOEBOORUCLIENT_H
#define MOEBOORUCLIENT_H

#include <curl/curl.h>
#include <QString>
#include <QThread>
#include <QDir>
#include <sstream>
#include "nwutils.h"
#include "curlresult.h"
#include "tvshow.h"
#include "filedownloadthread.h"
#include "wallpaperdownloadclient.h"

namespace Moebooru {
using WallpaperDownload::Entry;
using WallpaperDownload::FetchThread;
using WallpaperDownload::Rating;
using WallpaperDownload::SearchResult;

class Client : public WallpaperDownload::Client
{
public:
    Client(QString baseUrl, int limit = 10, Rating ratingFilter = WallpaperDownload::ratingSafe);

    SearchResult fetchPostsBlocking(QString tagName, int page = 1);
protected:
    CURL* curlClient(QString tag, CurlResult& userdata, const unsigned int page = 1);
};

}

#endif // MOEBOORUCLIENT_H
