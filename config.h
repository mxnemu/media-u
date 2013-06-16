#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QDir>

class Config
{
public:
    Config(QString initPath);

    bool init();
    bool init(QString path);
    bool parse(const QString &jsonData);
    bool createNewConfig(QString filepath);

    QString libraryPath();
    int serverPort();


    QDir configPath();
    QString malConfigFilePath();
    QString mplayerLocation();
    QString omxplayerLocation();
    bool omxPlayerIsInstalled();
    bool mplayerIsInstalled();
private:
    QString mConfigPath;
    QString mLibraryPath;
    int mServerPort;
    bool initialized;
};

#endif // CONFIG_H
