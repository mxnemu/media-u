#include "config.h"
#include <QFile>
#include <QDir>
#include <iostream>
#include <N0Slib.h>
#include "systemutils.h"

Config::Config(QString initPath)
{
    this->init(initPath);
    mServerPort = -1;
    initialized = false;
}

bool Config::init(QString path) {
    QDir dir;
    if (path.isNull() || path.isEmpty()) {
        dir = this->configPath();
        if (!dir.exists() && !dir.mkpath(dir.absoluteFilePath(".mediaU"))) {
            // TODO global error quit with dialogue message
            // home not writeable
        }
    } else {
        dir = QDir(path);
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
    // TODO parse json
    mLibraryPath = QString();
    std::string nwlibraryPath = libraryPath().toStdString();
    mServerPort = 8082;



    std::istringstream input(jsonData.toStdString());
    nw::JsonReader jr(input);
    jr.describe("port", mServerPort);

    jr.push("library");
    jr.describe("path", nwlibraryPath);
    jr.pop();

    jr.push("mplayer");
    mplayerConfig.describe(&jr);
    jr.pop();

    jr.close();

    mLibraryPath = QString(nwlibraryPath.data());

    initialized = true;
    return true;
}

bool Config::createNewConfig(QString filepath)
{
    QFile file(filepath);
    file.open(QFile::WriteOnly);
    file.write(QString(
        "{\n"
        "    \"port\":\"8082\",\n"
        "    \"library\": {\n"
        "        \"path\":\"%1\"\n"
        "    }\n"
        "}").arg(libraryPath()).toUtf8().data());
    file.close();

    std::cout << libraryPath().toStdString() << std::endl;

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

    QDir dir = QDir::home();
    return dir.absoluteFilePath(".mediaU/library");
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
    NwUtils::describe(*de, "path", path);
    NwUtils::describe(*de, "arguments", arguments);
    if (de->isInReadMode()) {
        this->postInit();
    }
}

void MplayerConfig::postInit() {
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
