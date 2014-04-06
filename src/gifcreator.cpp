#include "gifcreator.h"
#include "avconvutil.h"
#include "directoryscanner.h"
#include "filefilterscanner.h"
#include <QDir>
#include <QProcess>
#include <QDebug>

GifCreator::GifCreator(GifCreator::Config* config, QObject *parent) :
    ShortClipCreator(config, parent)
{
}

void GifCreator::init(QString videoPath, QString outputPath, float startSec, float endSec, std::pair<int,int> resolution, float maxSizeMib, int framesDropped) {
    this->videoPath = videoPath;
    this->outputPath = outputPath;
    this->startSec = startSec;
    this->endSec = endSec;
    this->resolution = resolution;
    this->maxSizeMib = maxSizeMib;
    this->framesDropped = std::max(framesDropped, 0);
}

bool GifCreator::generate() {
    QString dirPath = QDir::temp().absoluteFilePath(QString().sprintf("gif_%p", this));
    QDir dir(dirPath);
    dir.removeRecursively();
    if (!dir.mkpath(".")) {
        qDebug() << "failed to create tmp dir" << dirPath;
        return false;
    }

    float dif = endSec - startSec;
    if (dif > 120.f) {
        qDebug() << "gif longer than 120s canceling creation" << startSec << endSec;
        return false;
    }

    qDebug() << avconfutil::time(startSec) << avconfutil::time(dif);
    QProcess avconv;
    avconv.setWorkingDirectory(dirPath);
    avconv.start("avconv",
                 QStringList() <<
                 // fast seek
                 "-ss" <<
                 avconfutil::time((int)startSec) <<
                 "-i" <<
                 videoPath <<
                 // accurate seek
                 "-ss" <<
                 avconfutil::time(startSec-(float)((int)startSec)) <<
                 // relative dif
                 "-t" <<
                 avconfutil::time(dif) <<
                 "-s" <<
                 avconfutil::resolution(resolution.first, resolution.second) <<
                 "-f" <<
                 "image2" <<
                 "%03d.png"
                 );
    avconv.waitForFinished(-1);

    QList<QFileInfo> tmpFrames = QDir(dirPath).entryInfoList(QStringList() << "*.png", QDir::Files);
    QStringList frames;
    int iMod = 0;
    foreach (QFileInfo fileinfo, tmpFrames) {
        if (iMod == 0) {
            frames << fileinfo.filePath();
        }
        ++iMod;
        if (iMod >= framesDropped+1) {
            iMod = 0;
        }
    }

    int fps = (24 / (framesDropped+1)) + ((framesDropped > 0) ? 1 : 0);
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
    qint64 maxSize = (1024*(1024*maxSizeMib));
    if (maxSizeMib > 0 && size > maxSize) {
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
        this->resolution = lowerResolution;
        return this->generate();
    }

    QDir(QFileInfo(outputPath).dir()).mkpath(".");
    if (QFile::rename(tmpFilePath, outputPath)) {
        dir.removeRecursively();
        return true;
    }
    return false;
}


GifCreator::Config::Config() : ShortClipCreator::Config() {
    framesDropped = 2;
}
