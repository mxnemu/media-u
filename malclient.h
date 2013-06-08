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
#include "filedownloadthread.h"
#include "nwutils.h"

class MalClient : public QObject
{
    Q_OBJECT
public:
    explicit MalClient(QObject *parent = 0);
    void init(QString configFilePath);
    void setCredentials(const QString name, const QString password);
    bool verifyCredentials();
    
    //CurlXmlResult curlPerform(const char *url);
    bool hasValidCredentials() const;

    void fetchShows(QList<TvShow>& showList, QDir libraryDir);
    void fetchShowBlocking(TvShow &show, QDir libraryDir);
signals:
    void fetchingFinished();
    
private slots:
    void fetchThreadFinished();

private:
    CURL *curlClient(const char* url, CurlResult &userdata);

    bool mHasValidCredentials;
    QString username;
    QString password;
    //MalClientThread* activeThread;
};

class MalClientThread : public QThread {
public:
    MalClientThread(MalClient& client, QList<TvShow>& shows, QDir libraryDir);

    void run();
private:
    MalClient& malClient;
    QList<TvShow>& tvShows;
    QDir libraryDir;
};

class MalEntry {
public:
    friend class MalSearchResult;

    int querySimiliarityScore; // TODO not here
    void calculateQuerySimiliarity(const QString query);
private:
    MalEntry(nw::XmlReader& reader);
    void parse(nw::XmlReader& xr);
    void updateShowFromEntry(TvShow& show, QDir libraryDir) const;
    void parseSynonyms(nw::XmlReader &reader);

    QString id;
    QString title;
    QString englishTitle;
    QStringList synonyms;
    int episodes;
    QString type;
    QString status;
    QString startDate;
    QString endDate;
    QString synopsis;
    QString image;

    static QString dateFormat;
};

class MalSearchResult {
public:
    MalSearchResult(CurlResult& result, QString query);
    void parse(CurlResult& result);
    void updateShowFromBestEntry(TvShow& show, QDir libraryDir) const;
private:
    QList<MalEntry> entries;
    QString query;
};


#endif // MALCLIENT_H
