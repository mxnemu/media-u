#ifndef THUMBNAILCREATORAVCONV_H
#define THUMBNAILCREATORAVCONV_H

#include "thumbnailcreator.h"
#include <QProcess>
#include "config.h"

class ThumbCreationCallbackAvconv : public ThumbCreationCallback {
    Q_OBJECT
public:
    ThumbCreationCallbackAvconv(void* data, const AvconvConfig &);

    void start();

    QProcess process;
    QStringList args;

public slots:
    void processFinished(int);
private:
    const AvconvConfig& config;
};

class ThumbnailCreatorAvconv : public ThumbnailCreator {
public:
    ThumbnailCreatorAvconv(const AvconvConfig &config);

    ThumbCreationCallback* generateJpeg(QString file, int second, int width, int height, void *callbackData) const;
    ThumbCreationCallback* generatePng(QString file, int second, int width, int height, void *callbackData) const;

private:
    enum Format {
        invalid = 0,
        jpg,
        png
    };
    ThumbCreationCallback* generate(QString file, Format format, int second, int width, int height, void* callbackData) const;

    const AvconvConfig& config;
};

#endif // THUMBNAILCREATORAVCONV_H
