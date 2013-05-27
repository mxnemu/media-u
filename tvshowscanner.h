#ifndef TVSHOWSCANNER_H
#define TVSHOWSCANNER_H

#include "mediascanner.h"

class TvShowScanner : public MediaScanner
{
public:
    TvShowScanner(Library &library);
    void scanFiles(const QStringList &files, const QDir &dir);
};

#endif // TVSHOWSCANNER_H
