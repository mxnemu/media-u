#include "tvshowscanner.h"
#include "moviefile.h"
#include <iostream>

TvShowScanner::TvShowScanner(Library& library) : MediaScanner(library)
{
}

void TvShowScanner::scanFiles(const QStringList &files)
{
    foreach(QString file, files) {
        if (MovieFile::hasMovieExtension(file)) {
            std::cout << "movie file found " << file.toStdString() << std::endl;
            library.importTvShowEpisode(file);
        }
    }
}
