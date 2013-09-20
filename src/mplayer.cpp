#include "mplayer.h"
#include <QKeyEvent>
#include <QDir>
#include <QDebug>
#include "systemutils.h"

Mplayer::Mplayer(Library& library) :
    VideoPlayer(library)
{
    connect(&process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    connect(&process, SIGNAL(readyRead()), this, SLOT(onProcessOutput()));
}

Mplayer::~Mplayer() {

}

int Mplayer::playFileImpl(QString filepath) {
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
    emit playbackCanceled();
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
        emit playbackEndedNormally();
    }
    qDebug() << "mplayer closed with error code" << exitCode;
    paused = true;
}

void Mplayer::onProcessOutput() {
    QString output = this->process.readAll();
    QRegExp progressRegex("V:(\\s*)?([0-9\\.]*)\\s");
    if (-1 != output.indexOf(progressRegex)) {
        this->nowPlaying.seconds = progressRegex.cap(2).toFloat();
    }
}
