#include "onlinecredentials.h"

OnlineCredentials::OnlineCredentials() :
    mHasVerifiedCredentials(false)
{
}

CURL* OnlineCredentials::curlNoAuthClient(const char* url, CurlResult& userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, userAgent.toLatin1().data());
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    return handle;
}

CURL* OnlineCredentials::curlClient(const char* url, CurlResult& userdata) {
    CURL* handle = curlNoAuthClient(url, userdata);
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    return handle;
}

void OnlineCredentials::set(const QString name, const QString password) {
    this->username = name;
    this->password = password;
}


void OnlineCredentials::set(const QString name, const QString password, QString userAgent) {
    this->set(name, password);
    this->userAgent = userAgent;
}
