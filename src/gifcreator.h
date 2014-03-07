#ifndef GIFCREATOR_H
#define GIFCREATOR_H

#include <QThread>

class GifCreator : public QThread
{
    Q_OBJECT
public:
    explicit GifCreator(QObject *parent = 0);
    std::pair<int,int> suggestedResolution(std::pair<int,int> resolution);
    std::pair<int,int> suggestedResolution(int originalW, int originalH);
    void init(QString videoPath, QString outputPath, float startSec, float endSec, std::pair<int, int> resolution, float maxSizeMib = 3, int framesDropped = 2);
    void run();

    void generate();
signals:
    void done(bool);
public slots:
    
private:
    QString videoPath;
    QString outputPath; float startSec;
    float endSec;
    std::pair<int, int> resolution;
    float maxSizeMib;
    int framesDropped;
};

#endif // GIFCREATOR_H
