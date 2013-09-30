#include "directoryscanner.h"
#include <QDir>
#include <iostream>

DirectoryScanner::DirectoryScanner() {

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
        if (d == "." || d == "..") {
            continue;
        }

        QString dirPath = dir.absoluteFilePath(d);
        //std::cout << dirPath.toStdString() << std::endl;
        if (dirPath.startsWith(rootPath) && !handledDirs.contains(dirPath)) {
            handledDirs.append(dirPath);
            this->scan(dirPath, handledDirs, rootPath);
        }
    }

    const QStringList files = dir.entryList(QDir::Files);
    foreach (MediaScanner* scanner, mediaScanners) {
        scanner->scanFiles(files, dir);
    }
}

void DirectoryScanner::scan(QString path) {
    QStringList handledDirs;
    this->scan(path, handledDirs, QDir(path).absolutePath());
}

DirectoryScannerThread::DirectoryScannerThread(DirectoryScanner *scanner, const QList<SearchDirectory> &dirs, QObject *parent) :
    QThread(parent),
    scanner(scanner),
    dirs(dirs)
{
}

DirectoryScannerThread::~DirectoryScannerThread()
{
    delete this->scanner;
}

void DirectoryScannerThread::run() {
    for (int i=0; i < dirs.length(); ++i) {
        scanner->scan(dirs.at(i).dir.absolutePath());
    }
    emit done();
}
