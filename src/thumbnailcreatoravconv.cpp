#include "thumbnailcreatoravconv.h"
#include <QProcess>


ThumbnailCreatorAvconv::ThumbnailCreatorAvconv()
{
}

ThumbCreationCallback* ThumbnailCreatorAvconv::generateJpeg(QString file, int second, int width, int height, void* callbackData) const
{
    int formattedSecond = second % 60;
    int minute = (second / 60) % 60;
    int hour = (second / 60 / 60);

    ThumbCreationCallbackAvconv* tcc = new ThumbCreationCallbackAvconv(callbackData);

    tcc->process.start("avconv", QStringList() <<
        "-ss" <<
        QString("%1:%2:%3").arg(QString::number(hour), QString::number(minute), QString::number(formattedSecond)) <<
        "-t" <<
        "1" <<
        "-i" <<
        file <<
        "-s" <<
        QString("%1x%2").arg(QString::number(width), QString::number(height)) <<
        "-vframes" <<
        "1" <<
        "-f" <<
        "image2" <<
        "-"
    );

    QObject::connect(&tcc->process, SIGNAL(finished(int)), tcc, SLOT(processFinished(int)));
    return tcc;
    //return process.readAllStandardOutput();
}


ThumbCreationCallbackAvconv::ThumbCreationCallbackAvconv(void *data) : ThumbCreationCallback(data) {
}

void ThumbCreationCallbackAvconv::processFinished(int) {
    emit this->jpegGenerated(this->process.readAllStandardOutput());
}

