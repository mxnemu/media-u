#include "config.h"
#include <QFile>
#include <QDir>
#include <iostream>
#include <QDebug>
#include "systemutils.h"


Config::Config(QString initPath)
{
    mServerPort = -1;
    initialized = false;
    this->init(initPath);
}

bool Config::init(QString path) {
    QDir dir;
    if (path.isNull() || path.isEmpty()) {
        dir = this->configPath();
        if (!dir.exists() && !dir.mkpath(dir.absoluteFilePath(".mediaU"))) {
            throw "could not create config dir";
        }
    } else {
        dir = QDir(path);
        mConfigPath = path;
    }

    QFile configFile(dir.absoluteFilePath("config.json"));
    if (configFile.exists() && configFile.open(QFile::ReadOnly) && configFile.size() > 0) {
        return this->parse(configFile.readAll());
    } else {
        return this->createNewConfig(dir.absoluteFilePath("config.json"));
    }
}

bool Config::parse(const QString& jsonData)
{
    std::istringstream input(jsonData.toStdString());
    nw::JsonReader jr(input);
    this->describe(&jr);
    jr.close();

    initialized = true;
    return true;
}

void Config::describe(nw::Describer* de) {
    de->describe("port", mServerPort);

    de->push("library");
    NwUtils::describe(*de, "path", mLibraryPath);
    de->pop();

    de->push("mplayer");
    mplayerConfig.describe(de);
    de->pop();
}

bool Config::createNewConfig(QString filepath)
{
    mLibraryPath = libraryPath();
    mServerPort = 8082;

    nw::JsonWriter jw(filepath.toStdString());
    this->describe(&jw);
    jw.close();

    qDebug() << libraryPath();

    initialized = true;
    return true;
}

QDir Config::configPath() {
    if (mConfigPath.isNull() || mConfigPath.isEmpty()) {
        return QDir(QDir::home().absoluteFilePath(".mediaU"));
    }
    return QDir(mConfigPath);
}

QString Config::libraryPath() {
    if (initialized) {
        return this->mLibraryPath;
    }

    QDir dir = configPath();
    return dir.absoluteFilePath("library");
}

QString Config::malConfigFilePath() {
    return configPath().absoluteFilePath("malConfig.json");
}

QString Config::mplayerLocation() {
    return mplayerConfig.path;
}

QString Config::omxplayerLocation() {
    return "/usr/bin/omxplayer";
}

bool Config::omxPlayerIsInstalled() {
    return SystemUtils::commandExists(omxplayerLocation());
}

bool Config::mplayerIsInstalled() {
     return SystemUtils::commandExists(mplayerLocation());
}

int Config::serverPort() {
    if (initialized && mServerPort > 0) {
        return mServerPort;
    }
    return 8082;
}

void MplayerConfig::describe(nw::Describer *de) {
    if (de->isInWriteMode()) {
        this->initDefaultValues();
    }
    NwUtils::describe(*de, "path", path);
    NwUtils::describe(*de, "arguments", arguments, ' ');
    if (de->isInReadMode()) {
        this->initDefaultValues();
    }
}

void MplayerConfig::initDefaultValues() {
    if (this->path.isEmpty()) {
        this->path = "mplayer";
        this->arguments = QStringList() <<
             "-fs" <<
             "-ass" <<
#ifdef __linux__
             // workaround for a pulse audio bug
             "-ao" <<
             "alsa" <<
#endif
             "-embeddedfonts";
    }
}

const MplayerConfig& Config::getMplayerConfigConstRef() const
{
    return mplayerConfig;
}
