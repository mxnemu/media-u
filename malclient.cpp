#include "malclient.h"
#include <qhttpserver.h>
#include <qhttpconnection.h>
#include <QTcpSocket>
#include <QDebug>
#include <iostream>
#include <curl/curl.h>

MalClient::MalClient(QObject *parent) :
    QObject(parent)
{
}

void MalClient::setCredentials(const QString name, const QString password) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, "http://myanimelist.net/api/account/verify_credentials.xml");
    curl_easy_setopt(handle, CURLOPT_USERNAME, name.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, MalClient::write_data);

    CurlXmlResult userData(*this);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userData);
    CURLcode error = curl_easy_perform(handle);
    userData.print();
    std::cout << error << std::endl;

    curl_easy_cleanup(handle);
}

size_t MalClient::write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp) {
    CurlXmlResult* userData = static_cast<CurlXmlResult*>(userp);
    userData->data.write((const char*)buffer, characterSize*bufferSize);
    return bufferSize;
}
