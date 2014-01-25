#include "gifcreator.h"
#include <QDir>
#include <QProcess>
#include <QDebug>

GifCreator::GifCreator(QObject *parent) :
    QObject(parent)
{
}

void GifCreator::create(QString videoPath, int startSec, int endSec, float resizeFactor, int framesDropped) {
    QString dirPath = QDir::temp().absoluteFilePath(QString().sprintf("%p", this));

    if (!QDir(dirPath).mkpath(".")) {
        qDebug() << "failed to create tmp dir" << dirPath;
        return;
    }
    framesDropped = std::max(framesDropped, 0);

    QProcess ffmpeg;
    ffmpeg.setWorkingDirectory(dirPath);
    ffmpeg.start("ffmpeg",
                 QStringList() <<
                 "-ss" <<
                 "0:14:55" <<
                 "-i" <<
                 "video.mkv" <<
                 "-t" <<
                 "0:0:5" <<
                 "-s"
                 "480x270" <<
                 "-f" <<
                 "image2" <<
                 "%03d.png"
                 );
    ffmpeg.waitForFinished(0);

    QStringList tmpframes = QDir(dirPath).nameFiltersFromString("*.png");
    QStringList frames;
    int iMod = 0;
    foreach (QString file, tmpframes) {
        if (iMod == 0) {
            frames << file;
        }
        ++iMod;
        if (iMod >= framesDropped+1) {
            iMod = 0;
        }
    }

    int fps = 24 / (framesDropped+1);
    QString fpsString = QString("1x%1").arg(fps, 2, 10, QChar('0'));

    QProcess imagemagick;
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
}
