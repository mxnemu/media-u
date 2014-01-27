#ifndef GIFCREATOR_H
#define GIFCREATOR_H

#include <QObject>

class GifCreator : public QObject
{
    Q_OBJECT
public:
    explicit GifCreator(QObject *parent = 0);
    std::pair<int,int> suggestedResolution(std::pair<int,int> resolution);
    std::pair<int,int> suggestedResolution(int originalW, int originalH);
    void create(QString videoPath, QString outputPath, float startSec, float endSec, std::pair<int, int> resolution, int maxSizeMib = 3, int framesDropped = 2);
    
signals:
    
public slots:
    
};

#endif // GIFCREATOR_H
