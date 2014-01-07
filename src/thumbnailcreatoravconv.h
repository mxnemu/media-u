#ifndef THUMBNAILCREATORAVCONV_H
#define THUMBNAILCREATORAVCONV_H

#include "thumbnailcreator.h"
#include <QProcess>

class ThumbCreationCallbackAvconv : public ThumbCreationCallback {
    Q_OBJECT
public:
    ThumbCreationCallbackAvconv(void* data);

    void start();

    QProcess process;
    QStringList args;

public slots:
    void processFinished(int);
};

class ThumbnailCreatorAvconv : public ThumbnailCreator {
public:
    ThumbnailCreatorAvconv();

    ThumbCreationCallback* generateJpeg(QString file, int second, int width, int height, void *callbackData) const;
    ThumbCreationCallback* generatePng(QString file, int second, int width, int height, void *callbackData) const;

private:
    ThumbCreationCallback* generate(QString file, QString format, int second, int width, int height, void *callbackData) const;
    static const QString Png;
    static const QString Jpeg;
};

#endif // THUMBNAILCREATORAVCONV_H
