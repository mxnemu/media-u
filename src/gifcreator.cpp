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

void GifCreator::create(QString videoPath, int startSec, int endSec, int framesDropped) {
    QString dirPath = QDir::temp().absoluteFilePath(QString().sprintf("%p", this));
    QDir dir(dirPath);
    if (!dir.exists() && !dir.mkpath(".")) {
        qDebug() << "failed to create tmp dir" << dirPath;
        return;
    }
    framesDropped = std::max(framesDropped, 0);

    QProcess avconv;
    avconv.setWorkingDirectory(dirPath);
    avconv.start("avconv",
                 QStringList() <<
                 "-ss" <<
                 avconfutil::time(startSec) <<
                 "-i" <<
                 videoPath <<
                 "-t" <<
                 avconfutil::time(endSec - startSec) <<
                 "-s" <<
                 avconfutil::resolution(480, 270) <<
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

    int fps = 24 / (framesDropped+1);
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
    qDebug() << imagemagick.readAllStandardError();
    qDebug() << imagemagick.readAllStandardOutput();
}
