#ifndef FILEDOWNLOADTHREAD_H
#define FILEDOWNLOADTHREAD_H

#include <QThread>
#include <QFile>
#include "curlresult.h"


class FileDownloadThread : public QThread {
public:
    FileDownloadThread(QString url, QString downloadPath, bool overwriteExisting = true, bool keepOrignalName = false);
    void run();

    static int write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp);
signals:
    void preparedCurl(CURL*);

private:
    CURL* curlClient(const char *url, QFile &file);

    QString url;
    QString downloadPath;
    bool overwriteExisting;
    bool keepRemoteName;
};

#endif // FILEDOWNLOADTHREAD_H
