#ifndef TRANSMISSIONCLIENT_H
#define TRANSMISSIONCLIENT_H

#include <QProcess>
#include "torrentclient.h"

class TransmissionClient : public TorrentClient
{
    Q_OBJECT
public:
    explicit TransmissionClient(QObject *parent = 0);
    bool addTorrent(QString filePath);
    void newProcess();
signals:
    
public slots:

private:
    QString command;
    void findCommand();
    
    QProcess* process;
};

#endif // TRANSMISSIONCLIENT_H
