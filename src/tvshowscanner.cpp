#include "tvshowscanner.h"
#include "videofile.h"
#include "library.h"
#include "log.h"
#include <iostream>

TvShowScanner::TvShowScanner(Library& library, QObject* parent) :
    MediaScanner(parent),
    library(library)
{
}

void TvShowScanner::scanFiles(const QStringList &files, const QDir& dir)
{
    foreach(QString file, files) {
        if (VideoFile::hasVideoExtension(file)) {
            if (gLog.logFilesFound) { qDebug() << "movie file found " << file; }
            emit machingFile((dir.absoluteFilePath(file)));
        }
    }
}
