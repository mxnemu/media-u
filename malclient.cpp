#include "malclient.h"
#include <qhttpserver.h>
#include <qhttpconnection.h>
#include <QTcpSocket>

MalClient::MalClient(QObject *parent) :
    QObject(parent)
{
}

void MalClient::setCredentials(QString name, QString password) {
    QTcpSocket socket(this);
    socket.connectToHost("http://myanimelist.net/api/account/verify_credentials.xml ", 80);
    if (socket.waitForConnected()) {
        QHttpConnection connection(&socket);
        QHttpResponse* resp = new QHttpResponse(&connection);
        resp->setHeader("Credentials", "TODO base64 enc of user+password");
        resp->writeHead(200);
        resp->end();
        socket.close();
        connect(&connection, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
                this, SLOT(setCredentialsAnswer(QHttpRequest*, QHttpResponse*)));
        delete resp;
    }
}

void MalClient::setCredentialsAnswer(QHttpRequest* req, QHttpResponse* resp) {
    if (req->statusCode() == 200) {
        // all is well
    } else if (req->statusCode() == 204) {
        // TODO request new credentials
    } else {
        // WTF?
    }
}
