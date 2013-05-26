#include "malclient.h"
#include <qhttpserver.h>
#include <qhttpconnection.h>
#include <QTcpSocket>
#include <iostream>

MalClient::MalClient(QObject *parent) :
    QObject(parent)
{
}

void MalClient::setCredentials(const QString name, const QString password) {
    QTcpSocket* socket = new QTcpSocket(this);
    socket->connectToHost("www.myanimelist.net", 80);
    if (socket->waitForConnected()) {
        QHttpConnection* connection = new QHttpConnection(socket);
        QHttpResponse* resp = new QHttpResponse(connection);
        resp->setHeader("Credentials", QString("%1:%2").arg(name, password));
        resp->writeHead(200);
        resp->end();
        //socket->close();

        QString re(socket->readAll());
        std::cout << re.toStdString() << std::endl;

        connect(connection, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
                this, SLOT(setCredentialsAnswer(QHttpRequest*, QHttpResponse*)));
        //delete resp; // TODO delete with socket, since it belives to own the socket
        // TODO delete connection; yes we are indeed just leeking it right here. This is horrible



    } else {
        QString re(socket->readAll());
        std::cout << "failed to connect to setCredentials stuff:" << socket->errorString().toStdString() << std::endl;
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
