#include "videoclipcreator.h"
#include "avconvutil.h"
#include <QProcess>
#include <QDebug>

VideoClipCreator::VideoClipCreator(Config* config, QObject* parent) :
    ShortClipCreator(config, parent)
{
}

bool VideoClipCreator::generate() {
    const VideoClipCreator::Config* config = (VideoClipCreator::Config*)this->config;
    float dif = config->timeSpan();
    int rate = (config->maxSizeMib * 1024 * 8) / dif;

    QProcess process;
    QStringList args = (
        QStringList() <<
        avconfutil::fastAccurateSeekArgs(config->startSec, config->videoPath) <<
        "-t" <<
        avconfutil::time(dif) <<
        "-b:v" <<
        QString("%1K").arg(rate) <<
        //"-s" <<
        //avconfutil::resolution(config->resolution.first, config->resolution.second) <<
        (config->audioCodec.isEmpty() ? "-an" : config->audioCodec) <<
        config->videoCodecArgs() <<
        config->outputPath
    );
    process.start("avconv", args);
    process.waitForFinished(-1);
    qDebug() << process.readAllStandardError();
    return true;
}

VideoClipCreator::Config::Config() :
    ShortClipCreator::Config(),
    qualityCrf(12)
{
}

void VideoClipCreator::Config::describe(nw::Describer& de) {
    NwUtils::describe(de, "videoCodec", videoCodec);
    NwUtils::describe(de, "audioCodec", audioCodec);
    NwUtils::describe(de, "extension", extension);
    NwUtils::describe(de, "qualityCrf", qualityCrf);
}

QString VideoClipCreator::Config::getExtension() {
    return this->extension.isEmpty() ? this->extension : "webm";
}

QStringList VideoClipCreator::Config::videoCodecArgs() const {
    if (this->videoCodec.isEmpty()) {
        if (this->extension.isEmpty()) {
            return QStringList() << "-c:v" << "libvpx";
        }
        return QStringList();
    }
    return QStringList() << "-c:v" << this->videoCodec;
}
