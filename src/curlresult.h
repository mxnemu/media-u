#ifndef CURLRESULT_H
#define CURLRESULT_H

#include <curl/curl.h>
#include <sstream>
#include <QList>

class CurlResult {
public:
    CurlResult(void* userData = NULL);
    virtual ~CurlResult();
    std::stringstream data;
    void* userData;
    CURLcode curlError;
    void print();

    static size_t write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp);

    // This is fucking stupid and it makes no sense to
    // tack memory management onto the userdata, but whatever fuck the world
    QList<struct curl_slist*> slists; // memory management purposes
};

#endif // CURLRESULT_H
