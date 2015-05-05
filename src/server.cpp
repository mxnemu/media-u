#include "server.h"
#include <iostream>
#include <QFile>
#include <string>
#include <qhttprequest.h>
#include "systemutils.h"
#include "nwutils.h"

Server::Server(QString publicDirectoryPath, MainWindow &window, Library &library, VideoPlayer* player) :
    QObject(NULL), publicDirectory(publicDirectoryPath),
    window(window),
    library(library),
    player(player),
    pushEvents(library, *player)
{

}

std::pair<bool, int> Server::tryToBindPort(QHttpServer* server, int originalPort) {
    const int maxTries = 20; // try the 20 following ports
    bool success = false;
    int tryCount = 0;
    for (; (!success  && tryCount < maxTries); ++tryCount) {
        success = server->listen(originalPort + tryCount);
    }
    return std::make_pair(success, (originalPort + tryCount-1));
}

int Server::start(int serverPort) {
    this->server = new QHttpServer();
    std::pair<bool, int> boundPort = tryToBindPort(server, serverPort);

    if (!boundPort.first) {
        //delete this->server;
        return -1;
    }

    connect(this->server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));

    return boundPort.second;
}

void Server::handleRequest(QHttpRequest* req, QHttpResponse* resp) {
    if (req->path().contains(QRegExp("^\\/api\\/"))) {
        if (!handleApiRequest(req, resp)) {
            simpleWrite(resp, 405, "Api request not supported. Maybe a typo");
        }
    } else if (req->path().startsWith("/video/")) {
        streamVideo(req, resp);
    } else {
        sendFile(req, resp);
    }
}

bool Server::handleApiRequest(QHttpRequest* req, QHttpResponse* resp) {
    QString path = req->path();
    if (path.startsWith(QString("/api/setPage/"))) {
        QString pageName = QDir(req->path()).dirName();
        window.setPage(pageName, req->url().query(QUrl::FullyDecoded));
        simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"page\":\"%1\"}").arg(pageName), mime::json);
        return true;
    } else if (path.startsWith("/api/assurePage")) {
        QString pageName = QDir(req->path()).dirName();
        const QString query = req->url().query(QUrl::FullyDecoded);
        if (!window.getPage() || !window.getPage()->conformsTo(query)) {
            window.setPage(pageName, query);
            simpleWrite(resp, 200, QString("{\"status\":\"set\",\"page\":\"%1\"}").arg(pageName), mime::json);
        } else {
            simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"page\":\"%1\"}").arg(pageName), mime::json);
        }
        return true;
    } else if (path.startsWith("/api/activePage")) {
        simpleWrite(resp, 200, QString("{\"page\":\"%1\"}").arg(window.activePageId()), mime::json);
        return true;
    } else if (path.startsWith("/api/player/")) {
        return player->handleApiRequest(req, resp);
    } else if (path.startsWith("/api/library/")) {
        return library.handleApiRequest(req, resp);
    } else if (path.startsWith("/api/events/")) {
        return pushEvents.handleApiRequest(req, resp);
    } else if (path.startsWith("/api/page/") && window.getPage()) {
        return window.getPage()->handleApiRequest(req, resp);
    } else if (path.startsWith("/api/online/")) {
        return library.onlineSync.handleApiRequest(req, resp);
    }
    return false;
}

void Server::streamVideo(QHttpRequest* req, QHttpResponse* resp) {
    QString filePath = QString(req->url().path(QUrl::FullyDecoded)).remove(0, sizeof("/video") -1);
    QFile file(filePath);
    if (file.exists() && file.open(QFile::ReadOnly)) {
       QByteArray data = file.readAll();
       simpleWrite(resp, 200, QString(data), SystemUtils::fileMime(filePath));
       file.close();
    } else {
       simpleWrite(resp, 404, "File not found (Error 404)");
    }
}

void Server::sendFile(QHttpRequest* req, QHttpResponse* resp) {
    QDir localDir = QDir(this->publicDirectory.path().append(req->path()));

    QString filePath;
    if (localDir.exists()) {
        filePath = localDir.absoluteFilePath("index.html");
    } else {
        QString path = req->path();
        if (path.at(0) == '/') {
            path = path.right(path.length()-1);
        }
        filePath = this->publicDirectory.absoluteFilePath(path);
    }


    if (!filePath.startsWith(this->publicDirectory.absolutePath())) {
        simpleWrite(resp, 403, QString("Access denied. Only files in the public directory will be send."));
        return;
    }

    QFile file(filePath);
    if (file.exists() && file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        simpleWrite(resp, 200, QString(data), SystemUtils::fileMime(filePath));
        file.close();
    } else {
        simpleWrite(resp, 404, "File not found (Error 404)");
    }
}

void Server::sendRedirect(QHttpResponse* resp, const QString& location) {
    resp->setHeader("Location", location);
    resp->writeHead(301);
    resp->end();
}

void Server::simpleWrite(QHttpResponse* resp, int statusCode, const QString& data, QString mime) {
    simpleWriteBytes(resp, statusCode, data.toUtf8(), mime);
}

void Server::simpleWriteBytes(QHttpResponse* resp, int statusCode, const QByteArray& data, QString mime) {
    resp->setHeader("Content-Length", QString::number(data.size()));
    resp->setHeader("Content-Type", mime);
    resp->writeHead(statusCode);
    resp->write(data);
    resp->end();
}


RequestBodyListener::RequestBodyListener(QHttpResponse *resp, QObject* parent) :
    QObject(parent),
    resp(resp)
{
}

void RequestBodyListener::onDataReceived(QByteArray bytes) {
    QHttpRequest* req = dynamic_cast<QHttpRequest*>(sender());
    if (req) {
        emit bodyReceived(resp, bytes);
    } else {
        Server::simpleWrite(resp, 500, "Internal-Server-Error: wrong callback connected to bodyEnd()");
    }
    deleteLater();
}

ServerDataReady::ServerDataReady(QHttpResponse *resp, QObject* parent) :
    QObject(parent),
    resp(resp)
{
}

