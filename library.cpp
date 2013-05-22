#include "library.h"

Library::Library(QString path, QObject *parent) :
    QObject(parent),
    directory(path)
{
}

QString Library::randomWallpaperPath() const {
    // TODO debug test file; Impl actual fn
    return QString("/home/nehmulos/Downloads/test-wall.jpg");
}
