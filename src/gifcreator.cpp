#include "gifcreator.h"
#include "avconvutil.h"
#include "directoryscanner.h"
#include "filefilterscanner.h"
#include <QDir>
#include <QProcess>
#include <QDebug>

GifCreator::GifCreator(QObject *parent) :
    QObject(parent)
{
}

std::pair<int, int> GifCreator::suggestedResolution(std::pair<int, int> resolution) {
    return this->suggestedResolution(resolution.first, resolution.second);
}

std::pair<int, int> GifCreator::suggestedResolution(int originalW, int originalH) {
    originalW = originalW > 0 ? originalW : 480;
    originalH = originalH > 0 ? originalH : 253;
    int targetW = std::min(480, originalW);
    return std::pair<int,int>(targetW, ((float)originalH / (float)originalW) * (float)targetW);
}

void GifCreator::create(QString videoPath, QString outputPath, float startSec, float endSec, std::pair<int,int> resolution, int maxSizeMib, int framesDropped) {
    QString dirPath = QDir::temp().absoluteFilePath(QString().sprintf("gif_%p", this));
    QDir dir(dirPath);
    dir.removeRecursively();
    if (!dir.mkpath(".")) {
        qDebug() << "failed to create tmp dir" << dirPath;
        return;
    }
    framesDropped = std::max(framesDropped, 0);

    float dif = endSec - startSec;
    if (dif > 120.f) {
        qDebug() << "gif longer than 120s canceling creation" << startSec << endSec;
        return;
    }

    qDebug() << avconfutil::time(startSec) << avconfutil::time(dif);
    QProcess avconv;
    avconv.setWorkingDirectory(dirPath);
    avconv.start("avconv",
                 QStringList() <<
                 "-ss" <<
                 avconfutil::time(startSec) <<
                 "-i" <<
                 videoPath <<
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

    int fps = (24 / (framesDropped+1))+1;
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
    if (size > maxSize) {
        float scaleDownFactor = ((double)maxSize / (double)size) + 0.15;
        scaleDownFactor = std::min(scaleDownFactor, 0.9f);
        std::pair<int,int> lowerResolution = resolution;
        lowerResolution.first *= scaleDownFactor;
        lowerResolution.second *= scaleDownFactor;

        if (lowerResolution.first < 50 || lowerResolution.second < 50) {
            qDebug() << "gif is too long generation stopped. won't retry anymore.";
            return;
        }

        qDebug() << "gif was too big, retrying at resolution" << lowerResolution.first << "x" << lowerResolution.second;
        this->create(videoPath, outputPath, startSec, endSec, lowerResolution, maxSizeMib, framesDropped);
        return;
    }

    QDir(QFileInfo(outputPath).dir()).mkpath(".");
    if (QFile::rename(tmpFilePath, outputPath)) {
        dir.removeRecursively();
    }
}
