#ifndef THUMBNAILCREATOR_H
#define THUMBNAILCREATOR_H

#include <QObject>
#include <qpixmap.h>

// bind it's signal, then call start. It will report when it's done.
class ThumbCreationCallback : public QObject {
    Q_OBJECT
public:
    ThumbCreationCallback(void* data = NULL);
    void* data;

    virtual void start() = 0;
signals:
    // TODO rename to outputformat agnostic
    void jpegGenerated(const QByteArray img);
};

// Won't return actuall thumbnails, but callbacks
class ThumbnailCreator
{
public:
    ThumbnailCreator();

    //virtual QPixmap generate(QString file, int second, int width, int height) const;
    virtual ThumbCreationCallback* generateJpeg(QString file, int second, int width, int height, void *callbackData) const = 0;
    virtual ThumbCreationCallback* generatePng(QString file, int second, int width, int height, void *callbackData) const = 0;
};

#endif // THUMBNAILCREATOR_H
