#ifndef FILEFILTERSCANNER_H
#define FILEFILTERSCANNER_H

#include "mediascanner.h"

class FileFilterScanner : public MediaScanner
{
public:
    FileFilterScanner(QRegExp filterRegex);
    void scanFiles(const QStringList &files, const QDir &dir);

    QStringList getMatchedFiles() const;
private:
    QRegExp filterRegex;
    QStringList matchedFiles;
};

#endif // FILEFILTERSCANNER_H
