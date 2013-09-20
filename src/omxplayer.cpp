#include "omxplayer.h"

#include <QFile>
#include <QDebug>
#include "systemutils.h"

// TODO get rid of all the redunant stuff from mplayer and merge it into 1 base

Omxplayer::Omxplayer(Library& library) : VideoPlayer(library) {
}

int Omxplayer::playFileImpl(QString filepath) {
    if (!QFile::exists(filepath)) {
        qDebug() << "can not play: file does not exists. Is the drive connected?" << filepath;
    }
    paused = false;
    QStringList args;
    args.append("-t");
    args.append("0");
    args.append(filepath);
    process.start("omxplayer", args);
    process.waitForStarted();

    SystemUtils::setProcessPriority(process, -20);

    int error = process.error();
    if (process.state() == QProcess::NotRunning) {
        return error;
    }
    return 0;
}

void Omxplayer::pause() {
    if (!paused) {
        process.write("p");
        paused = true;
    }
}

void Omxplayer::unPause() {
    if (paused) {
        process.write("p");
        paused = false;
    }
}

void Omxplayer::stop() {
    process.write("q");
    paused = true;
}

void Omxplayer::backwards(const int seconds) {
    const int key = 0x5b44; // key left
    process.write((const char*)&key, sizeof(key));
}

void Omxplayer::forwards(const int seconds) {
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
