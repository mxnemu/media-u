#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class Config
{
public:
    Config(QString initPath);

    bool init(QString path);
    bool parse(const QString &jsonData);
    bool createNewConfig(QString filepath);

    QString libraryPath();
    int serverPort();

private:
    QString mLibraryPath;
    int mServerPort;
    bool initialized;
};

#endif // CONFIG_H
