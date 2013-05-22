#include "server.h"
#include <iostream>
#include <QFile>
#include <string>

Server::Server(QString publicDirectoryPath) : QObject(NULL), publicDirectory(publicDirectoryPath) {

}

int Server::start() {
    this->server = new QHttpServer();

    // TODO read from config json in release
    int port = 8082;
    while (!this->server->listen(QHostAddress(QHostAddress::Any), port)) {
        ++port;
    }

    connect(this->server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));
     std::cout << "oh hai" << std::endl;

     return port;
}

void Server::handleRequest(QHttpRequest* req, QHttpResponse* resp) {
    std::cout << "Connection" << std::endl;

    if (req->path().contains(QRegExp("^\\/api\\/"))) {
        std::cout << "Api request" << req->url().path().toUtf8().data() << std::endl;
        simpleWrite(resp, 405, "Api request not supported. Maybe a typo");
    } else {
        sendFile(req, resp);
    }
}

void Server::sendFile(QHttpRequest* req, QHttpResponse* resp) {
    QDir localDir = QDir(this->publicDirectory.path().append(req->path()));

    QString filePath;
    if (localDir.exists()) {
        std::cout << "dir exists sending index" << std::endl;
        filePath = localDir.filePath("index.html");
    } else {
        filePath = this->publicDirectory.filePath(req->path());
    }

    std::cout << "send file " << filePath.toStdString() <<" for url " << req->path().toUtf8().data() << std::endl;

    QFile file(filePath);
    if (file.exists() && file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        std::cout << "send data " << std::string(QString(data).toUtf8()) << std::endl;
        simpleWrite(resp, 200, QString(data));
    } else {
        simpleWrite(resp, 404, "File not found (Error 404)");
    }
}

void Server::simpleWrite(QHttpResponse* resp, int statusCode, const QString& data) {
    resp->setHeader("Content-Length", QString("%1").arg(data.size()));
    resp->writeHead(statusCode);
    resp->write(data);
    resp->end();
}
