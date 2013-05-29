#include "filedownloadthread.h"
#include <curl/curl.h>

FileDownloadThread::FileDownloadThread(QString url, QString downloadPath) :
    url(url),
    downloadPath(downloadPath)
{
}

void FileDownloadThread::run() {
    CurlResult result(this);
    CURL* handle = curlClient(url.toLocal8Bit().data(), result);
}

CURL* FileDownloadThread::curlClient(const char* url, CurlResult& result) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &result);
    //emit preparedCurl(handle); // TODO fix this to compile
    return handle;
}
