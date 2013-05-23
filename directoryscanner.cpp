#include "directoryscanner.h"
#include <QDir>
#include <iostream>

DirectoryScanner::DirectoryScanner(Library& library) : library(library) {

}

DirectoryScanner::~DirectoryScanner()
{
    foreach (MediaScanner* scanner, mediaScanners) {
        delete scanner;
    }
}

void DirectoryScanner::addScanner(MediaScanner *scanner) {
    if (scanner) {
        this->mediaScanners.append(scanner);
    }
}

void DirectoryScanner::scan(const QString& path, QStringList& handledDirs, const QString& rootPath) {
    QDir dir(path);
    QStringList dirs = dir.entryList(QDir::Dirs);
    foreach (QString d, dirs) {
        QString dirPath = dir.absoluteFilePath(d);
        std::cout << dirPath.toStdString() << std::endl;
        if (dirPath.startsWith(rootPath) && !handledDirs.contains(dirPath)) {
            handledDirs.append(dirPath);
            this->scan(dirPath, handledDirs, rootPath);
        }
    }

    const QStringList files = dir.entryList(QDir::Files);
    foreach (MediaScanner* scanner, mediaScanners) {
        scanner->scanFiles(files);
    }
}

void DirectoryScanner::scan(QString path) {
    QStringList handledDirs;
    this->scan(path, handledDirs, QDir(path).absolutePath());
}
