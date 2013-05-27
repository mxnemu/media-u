#include "malclient.h"
#include <qhttpserver.h>
#include <qhttpconnection.h>
#include <QTcpSocket>
#include <QDebug>
#include <iostream>

MalClient::MalClient(QObject *parent) :
    QObject(parent)
{
    mHasValidCredentials = false;
}

bool MalClient::setCredentials(const QString name, const QString password) {
    this->username = name;
    this->password = password;

    CurlXmlResult userData(*this);
    CURL* handle = curlClient("http://myanimelist.net/api/account/verify_credentials.xml", &userData);
    CURLcode error = curl_easy_perform(handle);
    userData.print();

    if (userData.data.str() == "Invalid credentials") {
        mHasValidCredentials = false;
    } else {
        mHasValidCredentials = true;
    }

    curl_easy_cleanup(handle);
    return mHasValidCredentials;
}

CURL* MalClient::curlClient(const char* url, void *userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, MalClient::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, userdata);
    return handle;
}

/*
CurlXmlResult MalClient::curlPerform(const char* url) {
    CurlXmlResult userData(this);
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, MalClient::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, userData);
    userData.curlError = curl_easy_perform(handle);
    return userData;
}
*/

size_t MalClient::write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp) {
    CurlXmlResult* userData = static_cast<CurlXmlResult*>(userp);
    userData->data.write((const char*)buffer, characterSize*bufferSize);
    return bufferSize;
}

bool MalClient::hasValidCredentials() const {
    return mHasValidCredentials;
}

