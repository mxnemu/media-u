#include "thumbnailcreatoravconv.h"
#include <QProcess>
#include <QDebug>


ThumbnailCreatorAvconv::ThumbnailCreatorAvconv()
{
}

ThumbCreationCallback* ThumbnailCreatorAvconv::generateJpeg(QString file, int second, int width, int height, void* callbackData) const {
    return this->generate(file, jpg, second, width, height, callbackData);
}

ThumbCreationCallback* ThumbnailCreatorAvconv::generatePng(QString file, int second, int width, int height, void* callbackData) const {
    return this->generate(file, png, second, width, height, callbackData);
}

ThumbCreationCallback* ThumbnailCreatorAvconv::generate(QString file, Format format, int second, int width, int height, void* callbackData) const {
    int formattedSecond = second % 60;
    int minute = (second / 60) % 60;
    int hour = (second / 60 / 60);

    ThumbCreationCallbackAvconv* tcc = new ThumbCreationCallbackAvconv(callbackData);

    QStringList& args = tcc->args;
    args.append("-ss");
    args.append(QString("%1:%2:%3").arg(QString::number(hour), QString::number(minute), QString::number(formattedSecond)));
    args.append("-t");
    args.append("1");
    args.append("-i");
    args.append(file);

    if (width != -1 && height != -1) {
        args.append("-s");
        args.append(QString("%1x%2").arg(QString::number(width), QString::number(height)));
    }
    args.append("-vframes");
    args.append("1");
    args.append("-f");
    args.append("image2");
    if (format == png) {
        args.append("-c");
        args.append("png");
    }
    args.append("-");

    //tcc->process.start("avconv", args);

    QObject::connect(&tcc->process, SIGNAL(finished(int)), tcc, SLOT(processFinished(int)));
    return tcc;
    //return process.readAllStandardOutput();
}


ThumbCreationCallbackAvconv::ThumbCreationCallbackAvconv(void *data) : ThumbCreationCallback(data) {
}

void ThumbCreationCallbackAvconv::start() {
    this->process.start("avconv", args);
    this->process.waitForFinished();
}

void ThumbCreationCallbackAvconv::processFinished(int) {
    emit this->jpegGenerated(this->process.readAllStandardOutput());
}

