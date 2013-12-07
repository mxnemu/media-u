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
    QString snapshotFormat;
    QString snapshotName;
private:
    void initDefaultValues(); ///< init uninitialized values
};

class Config
{
public:
    Config(int argc, char* argv[]);
    Config(QString initPath);
    void fromArgs(int argc, char* argv[]);

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

    bool getNoGui() const;
    bool getFullScreen() const;
    bool getAutoOpenBrowser() const;

private:
    QString mConfigPath;
    QString mLibraryPath;
    int mServerPort;
    bool noGui;
    bool fullScreen;
    bool autoOpenBrowser;

    bool initialized;

    MplayerConfig mplayerConfig;

    void setDefaults();
};

#endif // CONFIG_H
