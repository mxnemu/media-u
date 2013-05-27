#ifndef MEDIASCANNER_H
#define MEDIASCANNER_H

#include <QStringList>
#include "library.h"

class MediaScanner
{
public:
    MediaScanner(Library& library);
    virtual ~MediaScanner();
    virtual void scanFiles(const QStringList &files, const QDir &dir) = 0;
protected:
    Library& library;
};

#endif // MEDIASCANNER_H
