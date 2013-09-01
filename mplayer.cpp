#include "mplayer.h"
#include <QKeyEvent>
#include <QDir>
#include <QDebug>
#include "systemutils.h"

Mplayer::Mplayer() :
    VideoPlayer()
{
 // TODO connect finish slot
}

Mplayer::~Mplayer() {

}

int Mplayer::playFile(QString filepath) {
    if (!QFile::exists(filepath)) {
        qDebug() << "can not play: file does not exists. Is the drive connected?" << filepath;
    }
    QStringList args;
    args.append(QString("%1").arg(filepath));
    args.append("-fs");
    args.append("-ass");
    args.append("-embeddedfonts");
    args.append("-input");
    args.append(QString("conf=%1").arg(QDir::current().absoluteFilePath("mplayer.inputConfig")));
    process.start("mplayer", args);
    process.waitForStarted();

    SystemUtils::setProcessPriority(process, -20);

    if (process.state() == process.Running) {
        this->paused = false;
        playingFile = filepath;
    } else {
        paused = true;
    }

    return process.error();
}

void Mplayer::pause() {
    if (!paused) {
        process.write("p");
        paused = true;
    }
}

void Mplayer::unPause() {
    if (paused) {
        process.write("p");
        paused = false;
    }
}

void Mplayer::stop() {
    //process.kill();
    process.write("q");
    paused = true;
}

void Mplayer::backwards() {
    process.write("b");
}

void Mplayer::forwards() {
    process.write("f");
}

float Mplayer::incrementVolume() {
    process.write("*");
    return -1;
}

float Mplayer::decrementVolume() {
    process.write("/");
    return -1;
}

void Mplayer::onProcessFinished(int exitCode) {
    if (exitCode == 0) {

    }
    paused = true;
}
