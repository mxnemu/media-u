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

class MalClient;
class MalClientThread : public QThread {
public:
    MalClientThread(MalClient& client, QList<TvShow*>& shows, QDir libraryDir, QObject* parent);

    void run();
private:
    MalClient& malClient;
    QList<TvShow*>& tvShows;
    QDir libraryDir;
};


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

    void fetchShows(QList<TvShow *> &showList, QDir libraryDir);
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
    MalClientThread* activeThread;
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
    const MalEntry*bestResult() const;
private:
    QList<MalEntry> entries;
    QString query;
};

enum MalUpdaterWatchStatus {
    watching = 1,
    completed = 2,
    onhold = 3,
    dropped = 4,
    plantowatch = 6
};

class MalUpdaterAnimeData {
public:
    MalUpdaterAnimeData(TvShow*);
    static MalUpdaterWatchStatus calculateWatchStatus(int watched, int total);
private:
    int episode;
    MalUpdaterWatchStatus status; // int OR string. 1/watching, 2/completed, 3/onhold, 4/dropped, 6/plantowatch
    short score; // 0 - 10
    int downloaded_episodes;
    int storage_type; // int (will be updated to accomodate strings soon) // yeah sure soon...
    float storage_value; // wat
    int times_rewatched;
    short rewatch_value; // 0 - 10 ? dont know didn't check
    QDate date_start; // date. mmddyyyy
    QDate date_finish; // date. mmddyyyy
    int priority; // 0 - 10 ? dont know didn't check
    short enable_discussion; // int. 1=enable, 0=disable
    short enable_rewatching; // int. 1=enable, 0=disable
    QString comments; // free text field?
    QString fansub_group;
    QStringList tags; // string. tags separated by commas
};


#endif // MALCLIENT_H
