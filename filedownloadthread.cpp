#include "filedownloadthread.h"
#include <curl/curl.h>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

FileDownloadThread::FileDownloadThread(QString url, QString downloadPath) :
    url(url),
    downloadPath(downloadPath)
{
}

void FileDownloadThread::run() {
    QDir dir = QFileInfo(downloadPath).absoluteDir();
    if (!dir.exists()) {
        if (!QDir::root().mkpath(dir.absolutePath())) {
            qDebug() << "could not create director for fileDownload" << dir.absolutePath();
            deleteLater();
            return;
        }
    }

    QFile file(downloadPath);
    if (file.open(QFile::WriteOnly)) {
        CURL* handle = curlClient(url.toLocal8Bit().data(), file);
        int error = curl_easy_perform(handle);
        if (error) {
            qDebug() << "could not fetch file " << url;
        } else {
            qDebug() << "finished file-download of " << downloadPath;
        }
        file.close();
    } else {
        qDebug() << "could not write download to " << downloadPath;
    }
    deleteLater();
}

CURL* FileDownloadThread::curlClient(const char* url, QFile& file) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, FileDownloadThread::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &file);
    //emit preparedCurl(handle); // TODO fix this to compile
    return handle;
}

int FileDownloadThread::write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp)
{
       QFile* file = static_cast<QFile*>(userp);
       if (file) {
           return file->write((const char*)buffer, characterSize*bufferSize);
       }
       return 0;
}
