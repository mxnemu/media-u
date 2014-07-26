#ifndef ANIDBCREDENTIALS_H
#define ANIDBCREDENTIALS_H

#include "onlinecredentials.h"
#include <QUdpSocket>

class AnidbCredentials : public QObject, public OnlineCredentials {
    Q_OBJECT
public:
    explicit AnidbCredentials();

    class Response {
        Response();

        enum {
            Uninitialized = -1,
            UnknownCode = -2,
            NaN = -3,
            Ok = 200,
            OkAndNewVersionAvailable = 201,
            ClientDeprecated = 503,
            ClientBanned = 504,
            IllegalInputOrAcessDenied = 505,
            OutOfServiceTryLater = 601
        } TypeId;

        void parseDatagram(const QByteArray datagram);

        TypeId typeId;
        QString dataString;
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
