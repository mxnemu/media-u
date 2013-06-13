#include "mplayer.h"
#include <QKeyEvent>

Mplayer::Mplayer() :
    VideoPlayer()
{
}

void Mplayer::playFile(QString filepath) {
    paused = false;
    QStringList args;
    args.append(filepath);
    args.append("-fs");
    args.append("-ass");
    args.append("-embeddedfonts");
    process.start("mplayer", args);
}

void Mplayer::pause() {
    if (!paused) {
        process.write("p");
    }
}

void Mplayer::unPause() {
    if (paused) {
        process.write("p");
    }
}

void Mplayer::stop() {
    process.kill();
    paused = true;
}

// TODO doesn't work find a workaround for mplayer communication
void Mplayer::backwards() {
    static const QByteArray leftArrowKeyCode(1, Qt::Key_Left);
    process.write(leftArrowKeyCode); // left arrow key
}

void Mplayer::forwards() {
    static const QByteArray rightArrowKeyCode(1, Qt::Key_Right);
    process.write(rightArrowKeyCode); // right arrow key
}

float Mplayer::incrementVolume() {
    process.write("*");
    return -1;
}

float Mplayer::decrementVolume() {
    process.write("/");
    return -1;
}
