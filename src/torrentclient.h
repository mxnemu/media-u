#ifndef TORRENTCLIENT_H
#define TORRENTCLIENT_H

#include <QObject>

class TorrentClient : public QObject
{
    Q_OBJECT
public:
    explicit TorrentClient(QObject *parent = 0);
    virtual bool addTorrent(QString filePath) = 0;
signals:
    
public slots:
    
};

#endif // TORRENTCLIENT_H
