#include "mplayer.h"
#include <QKeyEvent>

Mplayer::Mplayer() :
    VideoPlayer()
{
 // TODO connect finish slot
}

Mplayer::~Mplayer() {
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
    //process.kill();
    process.write("q");
    paused = true;
}

// TODO doesn't work find a workaround for mplayer communication
void Mplayer::backwards() {
    const Qt::Key key = Qt::Key_Left;
    const QByteArray leftArrowKeyCode((const char*)&key, sizeof(key));
    process.write(leftArrowKeyCode); // left arrow key
}

void Mplayer::forwards() {
    const Qt::Key key = Qt::Key_Left;
    const QByteArray rightArrowKeyCode((const char*)&key, sizeof(key));
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

void Mplayer::onProcessFinished(int exitCode) {
    if (exitCode == 0) {

    }
    paused = true;
}
