#ifndef MALCLIENT_H
#define MALCLIENT_H

#include <QObject>
#include <qhttpresponse.h>
#include <qhttprequest.h>
#include <sstream>
#include <QDebug>
#include <curl/curl.h>
#include <QThread>
#include <tvshow.h>
#include <filedownloadthread.h>

class MalClient : public QObject
{
    Q_OBJECT
public:
    explicit MalClient(QObject *parent = 0);
    void init(QString configFilePath);
    bool setCredentials(const QString name, const QString password);
    
    //CurlXmlResult curlPerform(const char *url);
    bool hasValidCredentials() const;

    void fetchShows(QList<TvShow> showList);
    void fetchShowBlocking(TvShow &show);
signals:
    
private slots:

private:
    CURL *curlClient(const char* url, CurlResult &userdata);

    bool mHasValidCredentials;
    QString username;
    QString password;
    //MalClientThread* activeThread;
};

class MalClientThread : public QThread {
public:
    MalClientThread(MalClient& client, QList<TvShow> shows);

    void run();
private:
    MalClient& malClient;
    QList<TvShow> tvShows;
};

class MalEntry {
public:
    MalEntry(CurlResult& result);
    void parse(CurlResult& result);
    void updateShowFromEntry(TvShow& show);

    QString id;
    QString title;
    QString englishTitle;
    QString synonyms;
    int episodes;
    QString type;
    QString status;
    QString startDate;
    QString endDate;
    QString synopsis;
    QString image;

    static QString dateFormat;
};


#endif // MALCLIENT_H
