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
}

Mplayer::~Mplayer() {

}

bool Mplayer::playFileImpl(QString filepath, const TvShowPlayerSettings& settings) {
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
        if (diff > 2.f) {
            emit jumped(newTime);
        }
    } else {
        QRegExp snapshotRegex("screenshot '(.*)'");
        if (-1 != output.indexOf(snapshotRegex)) {
            QString snapshotName = snapshotRegex.cap(1);
            QString snapshotPath = QDir(config.snapshotDir).absoluteFilePath(snapshotName);
            QString outputName = snapshotOutputName(snapshotPath);
            if (snapshotPath != outputName) {
                unhandledSnapshots[snapshotPath] = outputName;
            }
        } else {
            qDebug() << output;
        }
    }
}

void Mplayer::convertSnapshots() {
    QStringList keys = unhandledSnapshots.keys();
    foreach (QString key, keys) {
        convertSnapshot(key, unhandledSnapshots.take(key));
    }
}

QString Mplayer::snapshotOutputName(QString) {
    //$(tvShow)/$(file) - at: $(progressM):$(progressS).$(ext)

    QString name = config.snapshotName;

    static const QString tvShowReplaceText("$(tvShow)");
    static const QString fileReplaceText("$(file)");
    static const QString progressMReplaceText("$(progressM)");
    static const QString progressSReplaceText("$(progressS)");
    static const QString nowDateReplaceText("$(nowDate)");
    static const QString extReplaceText("$(ext)");

    QString tvShowName = nowPlaying.tvShow ? nowPlaying.tvShow->name() : QString();
    QString minuteString = QString::number(nowPlaying.seconds / 60);
    QString secondString = QString::number(nowPlaying.seconds % 60);
    QString nowDateString = QString::number(QDateTime::currentMSecsSinceEpoch());

    name = name.replace(tvShowReplaceText, tvShowName);
    name = name.replace(fileReplaceText, QFileInfo(nowPlaying.path).fileName());
    name = name.replace(progressMReplaceText, minuteString);
    name = name.replace(progressSReplaceText, secondString);
    name = name.replace(nowDateReplaceText, nowDateString);
    name = name.replace(extReplaceText, config.snapshotFormat);

    QDir baseDir = QDir(config.snapshotDir);
    return baseDir.absoluteFilePath(name);
}

bool Mplayer::convertSnapshot(QString snapshotPath, QString outputPath) {
    QDir dir = QFileInfo(outputPath).dir();
    dir.mkpath(".");

    if (config.snapshotFormat == QFileInfo(snapshotPath).suffix()) {
        return QFile::rename(snapshotPath, outputPath);
    }

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
