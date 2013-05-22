#ifndef SERVER_H
#define SERVER_H

#include <qhttpserver.h>
#include <qhttpresponse.h>
#include <qhttprequest.h>


class Server : public QObject
{
    Q_OBJECT
public:
    Server();
    int start(); ///< returns the listening port
    static void simpleWrite(QHttpResponse *resp, int statusCode, const QString &data);


public slots:

    void handleRequest(QHttpRequest *req, QHttpResponse* resp);
private:
    QHttpServer* server;
    Q_DISABLE_COPY(Server)
};

#endif // SERVER_H
