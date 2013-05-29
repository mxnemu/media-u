#include "malclient.h"
#include <qhttpserver.h>
#include <qhttpconnection.h>
#include <QTcpSocket>
#include <QDebug>
#include <iostream>
#include <QFile>
#include "nwutils.h"

MalClient::MalClient(QObject *parent) :
    QObject(parent)
{
    mHasValidCredentials = false;
}

void MalClient::init(QString configFilePath) {
    if (QFile(configFilePath).exists()) {
        std::string user, password;

        nw::JsonReader jr(configFilePath.toStdString());
        jr.describe("user", user);
        jr.describe("password", password);
        jr.close();

        if (user.length() > 0 && password.length() > 0) {
            qDebug() << "mal connection is " << this->setCredentials(QString(user.data()), QString(password.data()));
        }
    }
}

void MalClient::fetchShows(QList<TvShow>showList) {
    MalClientThread* activeThread = new MalClientThread(*this, showList);
    activeThread->start(QThread::LowPriority);
}

void MalClient::fetchShowBlocking(TvShow& show) {
    QString name = show.name();
    if (name.isEmpty() || name.isNull()) {
        return;
    }

    CurlResult userData(this);
    const QString url = QString("http://myanimelist.net/api/anime/search.xml?q=").append(name);
    CURL* handle = curlClient(url.toLocal8Bit().data(), userData);
    CURLcode error = curl_easy_perform(handle);
    if (error) {
        qDebug() << "received error with this message:\n";
        userData.print();
    } else {
        MalEntry entry(userData);
        entry.updateShowFromEntry(show);
    }
}


bool MalClient::setCredentials(const QString name, const QString password) {
    this->username = name;
    this->password = password;

    CurlResult userData(this);
    CURL* handle = curlClient("http://myanimelist.net/api/account/verify_credentials.xml", userData);
    CURLcode error = curl_easy_perform(handle);
    if (error) {
        qDebug() << "received error with this message:\n";
        userData.print();
    }

    if (userData.data.str() == "Invalid credentials") {
        mHasValidCredentials = false;
    } else {
        mHasValidCredentials = true;
    }

    curl_easy_cleanup(handle);
    return mHasValidCredentials;
}

CURL* MalClient::curlClient(const char* url, CurlResult& userdata) {
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USERNAME, username.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_PASSWORD, password.toUtf8().data());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult::write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
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

bool MalClient::hasValidCredentials() const {
    return mHasValidCredentials;
}


MalClientThread::MalClientThread(MalClient &client, QList<TvShow> shows) :
    malClient(client),
    tvShows(shows)
{

}

void MalClientThread::run() {
    for (QList<TvShow>::iterator it = tvShows.begin(); it != tvShows.end(); ++it) {
        TvShow& show = it.i->t();
        malClient.fetchShowBlocking(show);
    }
}


MalEntry::MalEntry(CurlResult &result)
{
}

void MalEntry::parse(CurlResult &result) {
    nw::XmlReader xr(result.data);
    xr.push("anime");
    xr.describeArray("", "entry", 0);
    for (int i=0; xr.enterNextElement(i); ++i) {
        NwUtils::describe(xr, "id", id);
        NwUtils::describe(xr, "title", title);
        NwUtils::describe(xr, "englishTitle", englishTitle);
        NwUtils::describe(xr, "synonyms", synonyms);
        NwUtils::describe(xr, "episodes", episodes);
        NwUtils::describe(xr, "type", type);
        NwUtils::describe(xr, "status", status);
        NwUtils::describe(xr, "startDate", startDate);
        NwUtils::describe(xr, "endDate", endDate);
        NwUtils::describe(xr, "synopsis", synopsis);
        NwUtils::describe(xr, "image", image);
    }
}

QString MalEntry::dateFormat = "yyyy-MM-dd";

void MalEntry::updateShowFromEntry(TvShow &show) {
    //show.setName();
//    show.setLongTitle(title);
    show.setShowType(type);
    show.setAiringStatus(status);
    show.setStartDate(QDate::fromString(startDate, MalEntry::dateFormat));
    show.setEndDate(QDate::fromString(endDate, MalEntry::dateFormat));
    show.setSynopsis(synopsis);
    show.downloadImage(image);
}

