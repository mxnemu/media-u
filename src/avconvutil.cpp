#include "avconvutil.h"

namespace avconfutil {

QString time(int second) {
    int formattedSecond = second % 60;
    int minute = (second / 60) % 60;
    int hour = (second / 60 / 60);

    return QString("%1:%2:%3").arg(
                QString::number(hour),
                QString::number(minute),
                QString::number(formattedSecond)
                );
}

QString resolution(int width, int height) {
    return QString("%1x%2").arg(QString::number(width), QString::number(height));
}


} // namespace
