#include "videoclipcreator.h"
#include "avconvutil.h"
#include <QProcess>
#include <QDebug>
#include "config.h"

VideoClipCreator::VideoClipCreator(Config* config, const AvconvConfig &avconvConfig, QObject* parent) :
    ShortClipCreator(config, avconvConfig, parent)
{
}

bool VideoClipCreator::generate() {
    const VideoClipCreator::Config* config = (VideoClipCreator::Config*)this->config;
    float dif = config->timeSpan();
    int rate = ((config->maxSizeMib * 1024.f * 8.f) - config->audioRateKib) / dif;

    QProcess process;
    QStringList args = (
        QStringList() <<
        avconfutil::fastAccurateSeekArgs(config->startSec, config->videoPath) <<
        "-t" <<
        avconfutil::time(dif) <<
        "-b:v" <<
        QString("%1k").arg(rate)
        //"-s" <<
        //avconfutil::resolution(config->resolution.first, config->resolution.second) <<
    );

    if (config->audioRateKib != 0.f && !config->audioCodec.isEmpty()) {
        args << config->audioCodec;
    }

    if (config->audioRateKib == 0.f) {
        args << "-an";
    } else {
        args << "-b:a" << QString("%1k").arg((int)config->audioRateKib);
    }
    args << (config->videoCodecArgs() <<
             config->outputPath);
    process.start(avconvConfig.command, args);
    process.waitForFinished(-1);
    qDebug() << "create webm for: avconv" << args;
    qDebug() << process.readAllStandardError();
    return true;
}

VideoClipCreator::Config::Config() :
    ShortClipCreator::Config(),
    audioCodec(""),
    qualityCrf(12),
    audioRateKib(0.f)
{
}

void VideoClipCreator::Config::describe(nw::Describer& de) {
    ShortClipCreator::Config::describe(de);
    NwUtils::describe(de, "videoCodec", videoCodec);
    NwUtils::describe(de, "audioCodec", audioCodec);
    NwUtils::describe(de, "extension", extension);
    NwUtils::describe(de, "qualityCrf", qualityCrf);
}

QString VideoClipCreator::Config::getExtension() {
    return this->extension.isEmpty() ? this->extension : "webm";
}

void VideoClipCreator::Config::setAudioRateKib(float rateKib) {
    this->audioRateKib = std::max(84.f, rateKib); // for some reason avconv fails with vorbis rates < 84kib
}

QStringList VideoClipCreator::Config::videoCodecArgs() const {
    if (this->videoCodec.isEmpty()) {
        return QStringList();
    }
    return QStringList() << "-c:v" << this->videoCodec;
}
