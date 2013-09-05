#include "mplayer.h"
#include <QKeyEvent>
#include <QDir>
#include <QDebug>
#include "systemutils.h"

Mplayer::Mplayer() :
    VideoPlayer()
{
 // TODO connect finish slot

    connect(&process, SIGNAL(readyRead()), this, SLOT(onProcessOutput()));
}

Mplayer::~Mplayer() {

}

int Mplayer::playFile(QString filepath) {
    if (!QFile::exists(filepath)) {
        qDebug() << "can not play: file does not exists. Is the drive connected?" << filepath;
    }
    process.start("mplayer", QStringList() <<
        QString("%1").arg(filepath) <<
        "-fs" <<
        "-ass" <<
        "-embeddedfonts" <<
        "-slave"
    );
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
        process.write("pause\n");
        paused = true;
    }
}

void Mplayer::unPause() {
    if (paused) {
        process.write("pause\n");
        paused = false;
    }
}

void Mplayer::stop() {
    //process.kill();
    process.write("quit\n");
    paused = true;
}

void Mplayer::backwards(const int seconds) {
    process.write(QString("seek -%1\n").arg(seconds).toUtf8());
    paused = false;
}

void Mplayer::forwards(const int seconds) {
    process.write(QString("seek %1\n").arg(seconds).toUtf8());
    paused = false;
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

void Mplayer::onProcessOutput() {
    QString output = this->process.readAll();
    QRegExp progressRegex("V:(\\s*)?([0-9\\.]*)\\s");
    if (-1 != output.indexOf(progressRegex)) {
        this->progress = progressRegex.cap(2).toFloat();
    }
}
