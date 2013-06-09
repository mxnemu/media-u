#include "filefilterscanner.h"

FileFilterScanner::FileFilterScanner(QRegExp filterRegex) :
    filterRegex(filterRegex)
{
}

void FileFilterScanner::scanFiles(const QStringList &files, const QDir &dir) {
    for (int i=0; i < files.length(); ++i) {
        const QString& f = files.at(i);
        if (-1 != filterRegex.indexIn(f)) {
            matchedFiles.append(dir.absoluteFilePath(f));
        }
    }
}

QStringList FileFilterScanner::getMatchedFiles() const
{
    return matchedFiles;
}
