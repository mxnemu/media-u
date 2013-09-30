#include "tvshowscanner.h"
#include "moviefile.h"
#include <iostream>

TvShowScanner::TvShowScanner(Library& library, QObject* parent) :
    MediaScanner(parent),
    library(library)
{
}

void TvShowScanner::scanFiles(const QStringList &files, const QDir& dir)
{
    foreach(QString file, files) {
        if (MovieFile::hasMovieExtension(file)) {
            std::cout << "movie file found " << file.toStdString() << std::endl;
            library.importTvShowEpisode(dir.absoluteFilePath(file));
        }
    }
}
