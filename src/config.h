#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QDir>
#include "nwutils.h"
#include "videoclipcreator.h"
#include "gifcreator.h"

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

class BaseConfig
{
public:
    BaseConfig(int argc, char* argv[]);
    BaseConfig(QString initPath);
    void fromArgs(int argc, char* argv[]);

    bool init(QString path = QString());
    bool parse(const QString &jsonData);
    bool createNewConfig(QString filepath);
    void describe(nw::Describer *de);

    QString libraryPath();
    int serverPort();

    QDir configPath() const;
    QString malConfigFilePath() const;

    QString mplayerLocation();
    QString omxplayerLocation();
    bool omxPlayerIsInstalled();
    bool mplayerIsInstalled();

    std::pair<ShortClipCreator*, ShortClipCreator::Config*> cloneShortClipCreator() const;
    const SnapshotConfig& getSnapshotConfigConstRef() const;
    const MplayerConfig& getMplayerConfigConstRef() const;
    const RssConfig& getRssConfigConstRef() const;

    bool getNoGui() const;
    bool getFullScreen() const;
    bool getAutoOpenBrowser() const;

    const VideoClipCreator::Config& getVideoClipCreatorConfig() const;
    const GifCreator::Config& getGifCreatorConfig() const;

private:
    QString mConfigPath;
    QString mLibraryPath;
    QString shortClipCreatorType;

    int mServerPort;
    bool noGui;
    bool fullScreen;
    bool autoOpenBrowser;

    bool initialized;

    SnapshotConfig snapshotConfig;
    MplayerConfig mplayerConfig;
    RssConfig rssConfig;

    VideoClipCreator::Config videoClipCreatorConfig;
    GifCreator::Config gifCreatorConfig;

    void setDefaults();
};

#endif // CONFIG_H
