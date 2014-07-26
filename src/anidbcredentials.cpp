#include "anidbcredentials.h"
#include "config.h"
#include <qstringlist.h>

AnidbCredentials::AnidbCredentials() :
    QObject(),
    OnlineCredentials(),
    anidbAddress("api.anidb.net"),
    anidbPort(9000) // TODO allow override of address from config file
{

    connect(&socket, SIGNAL(readyRead()),
            this, SLOT(onUdpDataReceived()));
}

// AUTH user={str username}&pass={str password}&protover={int4 apiversion}
// &client={str clientname}&clientver={int4 clientversion}[&nat=1&comp=1
// &enc={str encoding}&mtu{int4 mtu value}&imgserver=1]
bool AnidbCredentials::verifyCredentials() {

    QString command =(QStringList()
        << QString("AUTH user=%1").arg(username)
        << QString("&pass=%1").arg(password)
        << QString("&protover=").arg(protocolVersion)
        << QString("&client=").arg(userAgent)
        << QString("&clientver=").arg(BaseConfig::SoftwareVersion)
        << QString("&nat=1")
        << QString("&comp=1")
        << QString("&enc=")
        //<< QString("&mtu=") default should be okay (1400)
        << QString("&imgserver=1]")).join("");

    socket.bind(8072);
    socket.writeDatagram(command.toUtf8(), anidbAddress, anidbPort);

    return false;
}


void AnidbCredentials::onUdpDataReceived()
{
    if (sender() != &socket) {
        return;
    }
    Response response;

    while (socket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket.pendingDatagramSize());
        QHostAddress senderAddress;
        quint16 senderPort;

        if (senderAddress != anidbAddress) {
            return;
        }

        socket.readDatagram(datagram.data(), datagram.size(),
                            &senderAddress, &senderPort);
        response.parseDatagram(datagram);

        //processTheDatagram(datagram);
    }
}

bool AnidbCredentials::loginResponse() {
/*
    200 {str session_key} LOGIN ACCEPTED
    201 {str session_key} LOGIN ACCEPTED - NEW VERSION AVAILABLE
    500 LOGIN FAILED
    503 CLIENT VERSION OUTDATED
    504 CLIENT BANNED - {str reason}
    505 ILLEGAL INPUT OR ACCESS DENIED
    601 ANIDB OUT OF SERVICE - TRY AGAIN LATER

 when nat=1

    200 {str session_key} {str ip}:{int2 port} LOGIN ACCEPTED
    201 {str session_key} {str ip}:{int2 port} LOGIN ACCEPTED - NEW VERSION AVAILABLE

 when imgserver=1

    200 {str session_key} LOGIN ACCEPTED

 {str image server name}

    201 {str session_key} LOGIN ACCEPTED - NEW VERSION AVAILABLE

 {str image server name}
 */
}

const int AnidbCredentials::protocolVersion = 3;


AnidbCredentials::Response::Response() :
    typeId(AnidbCredentials::Response::Uninitialized)
{
}

void AnidbCredentials::Response::parseDatagram(const QByteArray datagram) {
    if (!this->TypeId) {
        bool ok = false;
        // TODO figure out how to cast to enum without losing validitiy, resorting to a fallback enum value
        this->typeId = (TypeId)QString::fromUtf8(datagram).toInt(ok);
        if (!ok) {
            this->typeId = Response::NaN;
        }
    } else {
        this->dataString.fromUtf8(datagram);
    }
}
