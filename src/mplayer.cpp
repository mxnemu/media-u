#include "mplayer.h"
#include <QKeyEvent>
#include <QDir>
#include <QDebug>
#include "systemutils.h"

Mplayer::Mplayer(Library& library, const SnapshotConfig& snapshotConfig, const MplayerConfig &config) :
    VideoPlayer(library, snapshotConfig),
    config(config)
{
    connect(&process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    connect(&process, SIGNAL(readyRead()), this, SLOT(onProcessOutput()));
}

Mplayer::~Mplayer() {

}

bool Mplayer::playFileImpl(QString filepath, const TvShowPlayerSettings& settings) {
    process.setWorkingDirectory(snapshotConfig.snapshotDir);
    process.start(config.path, QStringList() <<
        QString("%1").arg(filepath) <<
        "-slave" <<
        config.arguments
    );
    process.waitForStarted();
    SystemUtils::setProcessPriority(process, -20);   

    if (process.state() == process.Running) {
        this->pauseStatus = false;

        if (settings.audioTrack != 0) {
            process.write(QString("switch_audio %1\n").arg(settings.audioTrack).toUtf8());
        }
        if (settings.subtileTrack != 0) {
            process.write(QString("sub_select %1\n").arg(settings.subtileTrack).toUtf8());
        }
        return true;
    }
    return false;
}

void Mplayer::pauseImpl() {
    if (!pauseStatus) {
        process.write("pause\n");
    }
}

void Mplayer::unPauseImpl() {
    if (pauseStatus) {
        process.write("pause\n");
    }
}

void Mplayer::stop() {
    //process.kill();
    process.write("quit\n");
    pauseStatus = true;
    emit playbackCanceled();
}

void Mplayer::backwards(const int seconds) {
    process.write(QString("seek -%1\n").arg(seconds).toUtf8());
}

void Mplayer::forwards(const int seconds) {
    process.write(QString("seek %1\n").arg(seconds).toUtf8());
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
    convertSnapshots();
    if (exitCode == 0) {
        emit playbackEndedNormally();
    }
    qDebug() << "mplayer closed with error code" << exitCode;
    pauseStatus = true;
}

void Mplayer::onProcessOutput() {
    QString output = this->process.readAll();
    QRegExp progressRegex("V:(\\s*)?([0-9\\.]*)\\s");
    if (-1 != output.indexOf(progressRegex)) {
        float newTime = progressRegex.cap(2).toFloat();
        float diff = std::abs((int)this->nowPlaying.seconds - (int)newTime);
        this->nowPlaying.seconds = newTime;
        if (pauseStatus) {
            process.write(QString("get_property pause\n").toUtf8());
        }
        if (diff > 2.f) {
            emit jumped(newTime);
        }
    } else if (!pauseStatus && (output.contains("PAUSE") || output.contains("ANS_pause=yes"))) {
        pauseStatus = true;
        emit paused();
    } else if (output.contains("ANS_pause=no")) {
        pauseStatus = false;
        emit unpaused();
    } else {
        QRegExp snapshotRegex("screenshot '(.*)'");
        if (-1 != output.indexOf(snapshotRegex)) {
            QString snapshotName = snapshotRegex.cap(1);
            QString snapshotPath = QDir(snapshotConfig.snapshotDir).absoluteFilePath(snapshotName);
            QString outputName = snapshotOutputPath();
            if (snapshotPath != outputName) {
                unhandledSnapshots[snapshotPath] = outputName;
            }
        } else {
            qDebug() << output;
        }
    }
}
