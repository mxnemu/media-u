#ifndef MALCLIENT_H
#define MALCLIENT_H

#include <QObject>
#include <qhttpresponse.h>
#include <qhttprequest.h>
#include <sstream>
#include <QDebug>
#include <curl/curl.h>

class MalClient : public QObject
{
    Q_OBJECT
public:
    explicit MalClient(QObject *parent = 0);

    bool setCredentials(const QString name, const QString password);
    

    static size_t write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp);
    //CurlXmlResult curlPerform(const char *url);
    bool hasValidCredentials() const;

signals:
    
private slots:

private:
    CURL *curlClient(const char* url, void *userdata);

    bool mHasValidCredentials;
    QString username;
    QString password;
};


class CurlXmlResult {
public:
    CurlXmlResult(MalClient& client) : client(client), curlError(CURL_LAST) {};
    std::stringstream data;
    MalClient& client;
    CURLcode curlError;
    void print() {qDebug() << data.str().data();};
};

#endif // MALCLIENT_H
