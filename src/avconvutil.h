#ifndef AVCONVUTIL_H
#define AVCONVUTIL_H

#include <QString>

namespace avconfutil {

QString time(float second);
QString resolution(int width, int height);
QStringList fastAccurateSeekArgs(float startSec, QString videoPath);

} // namespace

#endif // AVCONVUTIL_H
