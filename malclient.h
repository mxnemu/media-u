#ifndef MALCLIENT_H
#define MALCLIENT_H

#include <QObject>
#include <qhttpresponse.h>
#include <qhttprequest.h>
#include <sstream>
#include <QDebug>

class MalClient : public QObject
{
    Q_OBJECT
public:
    explicit MalClient(QObject *parent = 0);

    void setCredentials(const QString name, const QString password);
    

    static size_t write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp);
signals:
    
private slots:
    void setCredentialsAnswer(QHttpRequest *req, QHttpResponse *resp);
};

class CurlXmlResult {
public:
    CurlXmlResult(MalClient& client) : client(client) {};
    std::stringstream data;
    MalClient& client;
    void print() {qDebug() << data.str().data();};
};

#endif // MALCLIENT_H
