#ifndef GIFCREATOR_H
#define GIFCREATOR_H

#include <QObject>

class GifCreator : public QObject
{
    Q_OBJECT
public:
    explicit GifCreator(QObject *parent = 0);
    void create(QString videoPath, int startSec, int endSec, float resizeFactor = 0.5f, int framesDropped = 2);
    
signals:
    
public slots:
    
};

#endif // GIFCREATOR_H
