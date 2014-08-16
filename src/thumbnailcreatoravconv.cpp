#include "thumbnailcreatoravconv.h"
#include <QProcess>
#include <QDebug>
#include "avconvutil.h"


ThumbnailCreatorAvconv::ThumbnailCreatorAvconv(const AvconvConfig& config) :
    config(config)
{
}

ThumbCreationCallback* ThumbnailCreatorAvconv::generateJpeg(QString file, int second, int width, int height, void* callbackData) const {
    return this->generate(file, jpg, second, width, height, callbackData);
}

ThumbCreationCallback* ThumbnailCreatorAvconv::generatePng(QString file, int second, int width, int height, void* callbackData) const {
    return this->generate(file, png, second, width, height, callbackData);
}

ThumbCreationCallback* ThumbnailCreatorAvconv::generate(QString file, Format format, int second, int width, int height, void* callbackData) const {
    ThumbCreationCallbackAvconv* tcc = new ThumbCreationCallbackAvconv(callbackData, config);

    QStringList& args = tcc->args;
    args.append("-ss");
    args.append(avconfutil::time(second));
    args.append("-t");
    args.append("1");
    args.append("-i");
    args.append(file);

    if (width != -1 && height != -1) {
        args.append("-s");
        args.append(avconfutil::resolution(width, height));
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

    QObject::connect(&tcc->process, SIGNAL(finished(int)), tcc, SLOT(processFinished(int)));
    return tcc;
}


ThumbCreationCallbackAvconv::ThumbCreationCallbackAvconv(void *data, const AvconvConfig&) :
    ThumbCreationCallback(data),
    config(config)
{
}

void ThumbCreationCallbackAvconv::start() {
    this->process.start(config.command, args);
    this->process.waitForFinished();
}

void ThumbCreationCallbackAvconv::processFinished(int) {
    emit this->jpegGenerated(this->process.readAllStandardOutput());
}

