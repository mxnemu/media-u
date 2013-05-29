#ifndef CURLRESULT_H
#define CURLRESULT_H

#include <curl/curl.h>
#include <sstream>

class CurlResult {
public:
    CurlResult(void* userData = NULL);
    std::stringstream data;
    void* userData;
    CURLcode curlError;
    void print();

    static size_t write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp);
};

#endif // CURLRESULT_H
