#ifndef SERVER_H
#define SERVER_H

#include <qhttpserver.h>
#include <qhttpresponse.h>
#include <qhttprequest.h>

#include <QFile>
#include <QDir>


class Server : public QObject
{
    Q_OBJECT
public:
    Server(QString publicDirectoryPath);
    int start(); ///< returns the listening port
    static void simpleWrite(QHttpResponse *resp, int statusCode, const QString &data);
    void sendFile(QHttpRequest *req, QHttpResponse *resp);

public slots:
    void handleRequest(QHttpRequest *req, QHttpResponse* resp);

private:
    QHttpServer* server;
    QDir publicDirectory; ///< send files in this directory as HTTP GET responses
    Q_DISABLE_COPY(Server)
};

#endif // SERVER_H
