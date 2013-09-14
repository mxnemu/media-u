#ifndef MOVIESCANNER_H
#define MOVIESCANNER_H

#include <QString>
#include "library.h"
#include "mediascanner.h"

class DirectoryScanner
{
public:
    DirectoryScanner();
    virtual ~DirectoryScanner();
    void addScanner(MediaScanner* scanner);

    void scan(QString path);
    void scan(const QString& path, QStringList& handledDirs, const QString& rootPath);
    void scanFiles(const QStringList &files);
private:

    QList<MediaScanner*> mediaScanners;
};

#endif // MOVIESCANNER_H
