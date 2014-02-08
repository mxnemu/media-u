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
private:
    bool needsInit();
    void setDefaultValues();
};

class SnapshotConfig {
public:
    void describe(nw::Describer& de);
    QString snapshotDir;
    QString snapshotFormat;
    QString snapshotName;
    qint8 snapshotQuality;

    QString gifDir;
    QString gifName;
    int gifResolutionX;
    int gifResolutionY;
    float gifMaxSizeMiB;
    int gifFramesDropped;

private:
    bool needsInit();
    void setDefaultValues();
    QString createSaveDir(const QString parentDir, const QString dirname);
};

class RssConfig {
public:
    RssConfig();
    void describe(nw::Describer& de);
    bool autoDownload;
    bool requireFavouriteReleaseGroup;
    bool includeEnglish;
    bool includeRaw;
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


    const SnapshotConfig& getSnapshotConfigConstRef() const;
    const MplayerConfig& getMplayerConfigConstRef() const;
    const RssConfig& getRssConfigConstRef() const;

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

    SnapshotConfig snapshotConfig;
    MplayerConfig mplayerConfig;
    RssConfig rssConfig;

    void setDefaults();
};

#endif // CONFIG_H
