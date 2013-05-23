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
    int start(int serverPort); ///< returns the listening port
    bool handleApiRequest(QHttpRequest *req, QHttpResponse *resp);
    void sendFile(QHttpRequest *req, QHttpResponse *resp);


    static void simpleWrite(QHttpResponse *resp, int statusCode, const QString &data);
public slots:
    void handleRequest(QHttpRequest *req, QHttpResponse* resp);

private:
    QHttpServer* server;
    QDir publicDirectory; ///< send files in this directory as HTTP GET responses
    Q_DISABLE_COPY(Server)
};

#endif // SERVER_H
