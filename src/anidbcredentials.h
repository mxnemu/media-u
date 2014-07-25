#ifndef ANIDBCREDENTIALS_H
#define ANIDBCREDENTIALS_H

#include "onlinecredentials.h"
#include <QUdpSocket>

class AnidbCredentials : public QObject, public OnlineCredentials {
    Q_OBJECT
public:
    explicit AnidbCredentials();

    class Response {

    };
signals:

public slots:

private slots:
    void onUdpDataReceived();

protected:
    virtual bool verifyCredentials();

    bool loginResponse();

private:
    static const int protocolVersion;
    QUdpSocket socket;
    QHostAddress anidbAddress;
    int anidbPort;
};

#endif // ANIDBCREDENTIALS_H
