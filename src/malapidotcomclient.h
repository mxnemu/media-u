#ifndef MALAPIDOTCOMCLIENT_H
#define MALAPIDOTCOMCLIENT_H

#include <QObject>
#include <QThread>
#include "nwutils.h"
#include "curlresult.h"
#include "tvshow.h"
#include "onlinetvshowdatabase.h"

namespace MalApiDotCom {
using OnlineTvShowDatabase::UpdateFilter;
using OnlineTvShowDatabase::SearchResult;
using OnlineTvShowDatabase::Entry;

class Client : public OnlineTvShowDatabase::Client {
    Q_OBJECT
public:
    Client(QObject* parent = NULL);
};

}

#endif // MALAPIDOTCOMCLIENT_H
