#ifndef FILEDOWNLOADTHREAD_H
#define FILEDOWNLOADTHREAD_H

#include <QThread>
#include <QFile>
#include "curlresult.h"

// TODO I doubt that I need 1 thread for each download,
// but I could implement it without thinking
// prepare to get RAM-RAPED

class FileDownloadThread : public QThread {
    Q_OBJECT
public:
    FileDownloadThread(QString url, QString downloadPath, bool overwriteExisting = true, bool keepOrignalName = false);
    void run();

    static int write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp);
signals:
    void downloadSucceeded(QString path);

private:
    CURL* curlClient(const char *url, QFile &file);

    QString url;
    QString downloadPath;
    bool overwriteExisting;
    bool keepRemoteName;
};

#endif // FILEDOWNLOADTHREAD_H
