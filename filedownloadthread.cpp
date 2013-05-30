#include "filedownloadthread.h"
#include <curl/curl.h>
#include <QFile>
#include <QDebug>

FileDownloadThread::FileDownloadThread(QString url, QString downloadPath) :
    url(url),
    downloadPath(downloadPath)
{
}

void FileDownloadThread::run() {
    CurlResult result(this);
    CURL* handle = curlClient(url.toLocal8Bit().data(), result);
    int error = curl_easy_perform(handle);
    if (error) {
        qDebug() << "could not fetch file " << url;
        deleteLater();
        return;
    } else {
        QFile file(downloadPath);
        if (file.open(QFile::WriteOnly)) {
            // TODO use curl write_func to write chunks insead of a whole file
            // this can alloc a lot of of memory when downloading bigger files;
            std::string str = result.data.str();
            file.write(str.data());
            file.close();
        } else {
            qDebug() << "could not write download to " << downloadPath;
            deleteLater();
            return;
        }
    }
    qDebug() << "finished file-download of " << downloadPath;
    deleteLater();
}

CURL* FileDownloadThread::curlClient(const char* url, CurlResult& result) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &result);
    //emit preparedCurl(handle); // TODO fix this to compile
    return handle;
}
