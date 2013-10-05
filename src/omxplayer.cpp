#include "omxplayer.h"

#include <QFile>
#include <QDebug>
#include "systemutils.h"

// TODO get rid of all the redunant stuff from mplayer and merge it into 1 base

Omxplayer::Omxplayer(Library& library) : VideoPlayer(library) {
}

bool Omxplayer::playFileImpl(QString filepath, const TvShowPlayerSettings&) {
    if (!QFile::exists(filepath)) {
        qDebug() << "can not play: file does not exists. Is the drive connected?" << filepath;
    }
    pauseStatus = false;
    QStringList args;
    args.append("-t");
    args.append("0");
    args.append(filepath);
    process.start("omxplayer", args);
    process.waitForStarted();

    SystemUtils::setProcessPriority(process, -20);

    if (process.state() == QProcess::NotRunning) {
        return false;
    }
    return true;
}

void Omxplayer::pauseImpl() {
    if (!pauseStatus) {
        process.write("p");
        pauseStatus = true;
    }
}

void Omxplayer::unPauseImpl() {
    if (pauseStatus) {
        process.write("p");
    }
}

void Omxplayer::stop() {
    process.write("q");
    pauseStatus = true;
}

void Omxplayer::backwards(const int) {
    const int key = 0x5b44; // key left
    process.write((const char*)&key, sizeof(key));
}

void Omxplayer::forwards(const int) {
    const int key = 0x5b43; // key right
    process.write((const char*)&key, sizeof(key));
}

float Omxplayer::incrementVolume() {
    process.write("+");
    return -1;
}

float Omxplayer::decrementVolume() {
    process.write("-");
    return -1;
}
