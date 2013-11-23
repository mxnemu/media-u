#include "mplayer.h"
#include <QKeyEvent>
#include <QDir>
#include <QDebug>
#include "systemutils.h"

Mplayer::Mplayer(Library& library, const MplayerConfig &config) :
    VideoPlayer(library),
    config(config)
{
    connect(&process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    connect(&process, SIGNAL(readyRead()), this, SLOT(onProcessOutput()));

    connect(&fileSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onSnapshotReadyForConversion(QString)));
    connect(&fileSystemWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onSnapshotDirFileAdded(QString)));
}

Mplayer::~Mplayer() {

}

bool Mplayer::playFileImpl(QString filepath, const TvShowPlayerSettings& settings) {
    fileSystemWatcher.removePaths(fileSystemWatcher.directories());
    process.setWorkingDirectory(config.snapshotDir);
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
        fileSystemWatcher.addPath(config.snapshotDir);
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
        if (diff > 2.f) {
            emit jumped(newTime);
        }
    } else {
        QRegExp snapshotRegex("screenshot '(.*)'");
        if (-1 != output.indexOf(snapshotRegex)) {
            QString snapshotName = snapshotRegex.cap(1);
            QString snapshotPath = QDir(config.snapshotDir).absoluteFilePath(snapshotName);
            if (config.snapshotFormat != QFileInfo(snapshotName).completeSuffix()) {
                unhandledSnapshots[snapshotPath] = snapshotOutputName(snapshotPath);
            }
        } else {
            qDebug() << output;
        }
    }
}

void Mplayer::onSnapshotDirFileAdded(QString directory) {
    QStringList keys = unhandledSnapshots.keys();
    foreach (QString key, keys) {
        if (!convertSnapshot(key, unhandledSnapshots[key])) {
            fileSystemWatcher.addPath(key);
        } else {
            unhandledSnapshots.remove(key);
        }
    }
}

void Mplayer::onSnapshotReadyForConversion(QString file) {
    fileSystemWatcher.removePath(file);
    if (convertSnapshot(file, unhandledSnapshots[file])) {
        unhandledSnapshots.remove(file);
    }
}

// TODO put system time into the name to avoid overwriting
QString Mplayer::snapshotOutputName(QString snapshotPath) {
    return QString("%1.%2").arg(QDir(config.snapshotDir).absoluteFilePath(QFileInfo(snapshotPath).baseName()), config.snapshotFormat);
}

bool Mplayer::convertSnapshot(QString snapshotPath, QString outputPath) {
    QPixmap p;
    if (!p.load(snapshotPath)) {
        return false;
    }
    if (p.save(outputPath)) {
        if (!QFile(snapshotPath).remove()) {
            qDebug() << "failed to delete original snapshot" << snapshotPath;
        }
    } else {
        qDebug() << "failed to convert snapshot" << snapshotPath << "to" << outputPath;
    }
    return true;
}
