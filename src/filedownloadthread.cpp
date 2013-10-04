#include "filedownloadthread.h"
#include <curl/curl.h>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QUrl>

FileDownloadThread::FileDownloadThread(QString url, QString downloadPath, bool overwriteExisting, bool keepRemoteName) :
    url(url),
    downloadPath(downloadPath),
    overwriteExisting(overwriteExisting),
    keepRemoteName(keepRemoteName)
{
}

void FileDownloadThread::run() {
    // create dir
    QDir dir;
    if (keepRemoteName) {
        dir = QDir(downloadPath);
    } else {
        dir = QFileInfo(downloadPath).absoluteDir();
    }

    if (!dir.exists()) {
        if (!QDir::root().mkpath(dir.absolutePath())) {
            qDebug() << "could not create director for fileDownload" << dir.absolutePath();
            return;
        }
    }

    QString finalPath;
    QString filename;
    if (keepRemoteName) {
        filename = QFileInfo(QUrl(url).path()).fileName();
        finalPath = dir.absoluteFilePath(filename);
    } else {
        finalPath = downloadPath;
        filename = QFileInfo(downloadPath).fileName();
    }

    if (QFile(finalPath).exists() && !overwriteExisting) {
        return;
    }

    const QString tmpFilename(QString().sprintf("%p", this).append(filename));
    QFile tmpFile(QDir::temp().absoluteFilePath(tmpFilename));
    if (tmpFile.open(QFile::WriteOnly)) {
        CURL* handle = curlClient(url.toLocal8Bit().data(), tmpFile);
        int error = curl_easy_perform(handle);
        curl_easy_cleanup(handle);
        tmpFile.close();

        if (error) {
            qDebug() << "could not fetch file " << url;
            tmpFile.remove();
        } else {
            qDebug() << "finished file-download of " << finalPath;
            emit downloadSucceeded(finalPath);
        }


        if (!tmpFile.rename(finalPath)) {
            qDebug() << "could not rename downloaded tmp file to destination file: " << finalPath;
            tmpFile.remove();
        }
    } else {
        qDebug() << "could not write download to " << finalPath;
    }
}

CURL* FileDownloadThread::curlClient(const char* url, QFile& file) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, FileDownloadThread::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
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
