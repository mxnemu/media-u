#include "curlresult.h"

#include <QDebug>

CurlResult::CurlResult(void* userData) :
    userData(userData),
    curlError(CURL_LAST)
{
}

size_t CurlResult::write_data(void *buffer, size_t characterSize, size_t bufferSize, void *userp) {
    CurlResult* userData = static_cast<CurlResult*>(userp);
    if (userData) {
        userData->data.write((const char*)buffer, characterSize*bufferSize);
        return bufferSize;
    }
    return 0;
}

void CurlResult::print() {
    qDebug() << data.str().data();
}
