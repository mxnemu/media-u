#include "moviefile.h"

MovieFile::MovieFile(QObject *parent) :
    QObject(parent)
{
}

bool MovieFile::hasMovieExtension(QString filename) {
    return filename.contains(QRegExp("\\.mkv|\\.ogv|\\.mpeg|\\.mp4|\\.webm|\\.avi", Qt::CaseInsensitive));
}
