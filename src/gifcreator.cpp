#include "gifcreator.h"
#include "avconvutil.h"
#include "directoryscanner.h"
#include "filefilterscanner.h"
#include <QDir>
#include <QProcess>
#include <QDebug>
#include "config.h"

GifCreator::GifCreator(GifCreator::Config* config, const AvconvConfig& avconvConfig, QObject *parent) :
    ShortClipCreator(config, avconvConfig, parent)
{
}

bool GifCreator::generate() {
    return generate(this->config->resolution);
}


bool GifCreator::generate(std::pair<int,int> resolution) {
    QString dirPath = QDir::temp().absoluteFilePath(QString().sprintf("gif_%p", this));
    QDir dir(dirPath);
    dir.removeRecursively();
    if (!dir.mkpath(".")) {
        qDebug() << "failed to create tmp dir" << dirPath;
        return false;
    }

    float dif = config->endSec - config->startSec;
    if (dif > 120.f) {
        qDebug() << "gif longer than 120s canceling creation" << config->startSec << config->endSec;
        return false;
    }

    qDebug() << avconfutil::time(config->startSec) << avconfutil::time(dif);

    QStringList args = (QStringList() <<
        // fast seek
        "-ss" <<
        avconfutil::time((int)config->startSec) <<
        "-i" <<
        config->videoPath <<
        // accurate seek
        "-ss" <<
        avconfutil::time(config->startSec-(float)((int)config->startSec)) <<
        // relative dif
        "-t" <<
        avconfutil::time(dif) <<
        "-s" <<
        avconfutil::resolution(resolution.first, resolution.second) <<
        "-f" <<
        "image2" <<
        "%03d.png");

    qDebug() << args;

    QProcess process;
    process.setWorkingDirectory(dirPath);
    process.start(avconvConfig.command, args);
    process.waitForFinished(-1);

    const GifCreator::Config* gifConfig = dynamic_cast<const GifCreator::Config*>(config);
    QList<QFileInfo> tmpFrames = QDir(dirPath).entryInfoList(QStringList() << "*.png", QDir::Files);
    QStringList frames;
    int iMod = 0;
    foreach (QFileInfo fileinfo, tmpFrames) {
        if (iMod == 0) {
            frames << fileinfo.filePath();
        }
        ++iMod;
        if (iMod >= gifConfig->framesDropped+1) {
            iMod = 0;
        }
    }

    int fps = (24 / (gifConfig->framesDropped+1)) + ((gifConfig->framesDropped > 0) ? 1 : 0);
    QString fpsString = QString("1x%1").arg(fps, 2, 10, QChar('0'));

    QProcess imagemagick;
    imagemagick.setWorkingDirectory(dirPath);
    imagemagick.start("convert",
                      QStringList() <<
                      "+dither" <<
                      "-delay" <<
                      fpsString <<
                      //"-resize" <<
                      //"720x405" <<
                      frames <<
                      "-coalesce" <<
                      "-layers" <<
                      "OptimizeTransparency" <<
                      "out.gif"
                      );
    imagemagick.waitForFinished(-1);

    QString tmpFilePath = dir.absoluteFilePath("out.gif");

    qint64 size = QFile(tmpFilePath).size();
    qint64 maxSize = (1024*(1024*config->maxSizeMib));
    if (config->maxSizeMib > 0 && size > maxSize) {
        float scaleDownFactor = ((double)maxSize / (double)size) + 0.15;
        scaleDownFactor = std::min(scaleDownFactor, 0.9f);
        std::pair<int,int> lowerResolution = resolution;
        lowerResolution.first *= scaleDownFactor;
        lowerResolution.second *= scaleDownFactor;

        if (lowerResolution.first < 50 || lowerResolution.second < 50) {
            qDebug() << "gif is too long generation stopped. won't retry anymore.";
            return false;
        }

        qDebug() << "gif was too big, retrying at resolution" << lowerResolution.first << "x" << lowerResolution.second;
        return this->generate(lowerResolution);
    }

    QDir(QFileInfo(config->outputPath).dir()).mkpath(".");
    if (QFile::rename(tmpFilePath, config->outputPath)) {
        dir.removeRecursively();
        return true;
    }
    return false;
}


GifCreator::Config::Config() :
    ShortClipCreator::Config(),
    framesDropped(2)
{
}

void GifCreator::Config::describe(nw::Describer& de) {
    ShortClipCreator::Config::describe(de);
    NwUtils::describe(de, "resolutionX", resolutionX);
    NwUtils::describe(de, "resolutionY", resolutionY);
    NwUtils::describe(de, "framesDropped", framesDropped);
}
