#include "maldropurl.h"
#include "malclient.h"

MalDropUrl::MalDropUrl(QObject *parent) :
    OnlineDropUrl(parent)
{
}

bool MalDropUrl::handleUrl(TvShow* show, const QUrl url) {
    if (url.host() != "myanimelist.net") {
        return false;
    }

    QRegExp remoteIdRegex("^/anime/([0-9]+)(/|$)");
    int index = url.path().indexOf(remoteIdRegex);
    if (index == -1 || remoteIdRegex.captureCount() < 1) {
        return false;
    }

    bool parseOk = false;
    int remoteId = remoteIdRegex.cap(1).toInt(&parseOk);
    if (!parseOk) {
        return false;
    }

    show->setRemoteId(Mal::Client::IDENTIFIER_KEY, remoteId);
    return true;
}
