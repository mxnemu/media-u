#include "mplayer.h"

Mplayer::Mplayer() :
    VideoPlayer()
{
}

void Mplayer::playFile(QString filepath) {
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
}

float Mplayer::incrementSound() {
    process.write("*");
    return -1;
}

float Mplayer::decrementSound() {
    process.write("/");
    return -1;
}
