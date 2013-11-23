#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QDir>
#include "nwutils.h"

class MplayerConfig {
public:
    void describe(nw::Describer*de);
    QString path;
    QStringList arguments;
    QString snapshotDir;
private:
    void initDefaultValues(); ///< init uninitialized values
};

class Config
{
public:
    Config(QString initPath);

    bool init(QString path = QString());
    bool parse(const QString &jsonData);
    bool createNewConfig(QString filepath);
    void describe(nw::Describer *de);

    QString libraryPath();
    int serverPort();

    QDir configPath();
    QString malConfigFilePath();

    QString mplayerLocation();
    QString omxplayerLocation();
    bool omxPlayerIsInstalled();
    bool mplayerIsInstalled();


    const MplayerConfig& getMplayerConfigConstRef() const;

private:
    QString mConfigPath;
    QString mLibraryPath;
    int mServerPort;
    bool initialized;

    MplayerConfig mplayerConfig;
};

#endif // CONFIG_H
