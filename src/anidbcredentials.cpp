#include "anidbcredentials.h"
#include "config.h"
#include <qstringlist.h>
#include <QUdpSocket>

AnidbCredentials::AnidbCredentials() :
    OnlineCredentials()
{
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

    QUdpSocket socket;
    //socket.write()

    return false;
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
