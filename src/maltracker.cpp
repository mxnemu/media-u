#include "maltracker.h"

namespace Mal {

Tracker::Tracker(QObject *parent) :
    OnlineTracker(parent)
{
}

CURL* Tracker::curlTrackerUpdateClient(const char* url, CurlResult& userdata, AnimeUpdateData& data) {
    CURL* handle = curlClient(url, userdata);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, true);
    QString dataStr = QUrl(data.toXml()).toEncoded();
    QByteArray xml = QString("data=%1").arg(dataStr).toUtf8();
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, xml.size());
    curl_easy_setopt(handle, CURLOPT_COPYPOSTFIELDS, xml.data());
    return handle;
}

bool Client::fetchOnlineTrackerList(QList<TvShow*>& shows) {

    QString url = QString("http://myanimelist.net/malappinfo.php?u=%1&status=all&type=anime").arg(username);
    CurlResult userData(this);

    CURL* handle = curlNoAuthClient(url.toUtf8().data(), userData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for MAL Online Tracker Update '" << url << "'' with this message:\n";
        userData.print();
    } else {
        nw::XmlReader xr(userData.data);
        animeListData.describe(xr);
        if (!animeListData.error.isEmpty()) {
            qDebug() << "got error from mal status list fetching:" << animeListData.error;
            return false;
        }
        animeListData.updateShows(shows);
        return true;
    }
    return false;
}

bool Client::updateInOnlineTracker(TvShow* show) {
    int id = show->getRemoteId();
    if (id <= 0) return false;

    if (!animeListData.error.isEmpty()) {
        return false;
    }

    const AnimeItemData* item = animeListData.getShow(show);
    if (item) {
        if (item->localIsUpToDate(show) && !item->remoteIsUpToDate(show)) {
            if (item->remoteIsEq(show)) {
                show->setLastOnlineTrackerUpdate(item->my_last_updated);
                return true;
            }
            return this->updateinOnlineTrackerOrAdd(show, "update");
        }
        qDebug() << "MAL TRACKER skip up2date" << show->name();
        return true;
    } else {
        return this->updateinOnlineTrackerOrAdd(show, "add");
    }
}

bool Client::updateinOnlineTrackerOrAdd(TvShow* show, const QString& type) {
    QString url = QString("http://myanimelist.net/api/animelist/%2/%1.xml").arg(QString::number(show->getRemoteId()), type);
    CurlResult userData(this);
    AnimeUpdateData updateData(show);

    CURL* handle = curlTrackerUpdateClient(url.toUtf8().data(), userData, updateData);
    CURLcode error = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (error || userData.data.str().size() < 2) {
        qDebug() << "received error" << error << "for MAL Online Tracker Update '" << url << "'' with this message:\n";
        userData.print();
    } else {
        if (type == "update" && userData.data.str() == "Updated") {
            show->setLastOnlineTrackerUpdate(QDateTime::currentDateTimeUtc());
            qDebug() << "MAL TRACKER UPDATE success" << show->name();
            return true;
        } else if (type == "add") {
            QString responseString = userData.data.str().c_str();
            if (responseString.contains("201 Created")) {
                show->setLastOnlineTrackerUpdate(QDateTime::currentDateTimeUtc());
                qDebug() << "MAL TRACKER ADD success" << show->name() << QDateTime::currentDateTimeUtc();
                return true;
            }
        }
    }
    qDebug() << "Could not" << type << "MAL tracker:\n";
    userData.print();
    return false;
}



}
