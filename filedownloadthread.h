#ifndef FILEDOWNLOADTHREAD_H
#define FILEDOWNLOADTHREAD_H

#include <QThread>
#include "curlresult.h"

class FileDownloadThread : public QThread {
public:
    FileDownloadThread(QString url, QString downloadPath);
    void run();

signals:
    void preparedCurl(CURL*);

private:
    CURL* curlClient(const char *url, CurlResult &result);

    QString url;
    QString downloadPath;
};

#endif // FILEDOWNLOADTHREAD_H
