#include "server.h"
#include <iostream>

Server::Server() : QObject(NULL)
{

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
    simpleWrite(resp, 200, "Hello");
}


void Server::simpleWrite(QHttpResponse* resp, int statusCode, const QString& data) {
    resp->setHeader("Content-Length", QString("%1").arg(data.size()));
    resp->writeHead(statusCode);
    resp->write(data);
    resp->end();
}
