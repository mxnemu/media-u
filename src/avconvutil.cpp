#include "avconvutil.h"
#include <QStringList>

namespace avconfutil {

QString time(float second) {
    int baseSecond = second;
    int formattedSecond = baseSecond % 60;
    int minute = (baseSecond / 60) % 60;
    int hour = (baseSecond / 60 / 60);
    float remains = second - baseSecond;

    if (remains > 0) {
        return QString("%1:%2:%3").arg(
                    QString::number(hour),
                    QString::number(minute),
                    QString::number((float)formattedSecond + remains)
                    );
    } else {
        return QString("%1:%2:%3").arg(
                    QString::number(hour),
                    QString::number(minute),
                    QString::number(formattedSecond)
                    );
    }
}

QString resolution(int width, int height) {
    return QString("%1x%2").arg(QString::number(width), QString::number(height));
}

QStringList fastAccurateSeekArgs(float startSec, QString videoPath) {
    return QStringList() <<
        // fast seek (before -i)
        "-ss" <<
        avconfutil::time((int)startSec) <<
        "-i" <<
        videoPath <<
        // accurate seek (after -i)
        "-ss" <<
        avconfutil::time(startSec - (float)((int)startSec));
}


} // namespace
