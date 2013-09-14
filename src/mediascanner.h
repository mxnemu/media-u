#ifndef MEDIASCANNER_H
#define MEDIASCANNER_H

#include <QStringList>
#include "library.h"

class MediaScanner
{
public:
    MediaScanner();
    virtual ~MediaScanner();
    virtual void scanFiles(const QStringList &files, const QDir &dir) = 0;
};

#endif // MEDIASCANNER_H
