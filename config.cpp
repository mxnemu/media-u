#include "config.h"
#include <QFile>
#include <QDir>
#include <iostream>
#include <N0Slib.h>

Config::Config(QString initPath)
{
    this->init(initPath);
    mServerPort = -1;
    initialized = false;
}

bool Config::init() {
    return this->init(QString());
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
    return "/usr/bin/mplayer";
}

QString Config::omxplayerLocation() {
    return "/usr/bin/omxplayer";
}

bool Config::omxPlayerIsInstalled() {
    return QFile::exists(omxplayerLocation());
}

bool Config::mplayerIsInstalled() {
    return QFile::exists(mplayerLocation());
}

int Config::serverPort() {
    if (initialized && mServerPort > 0) {
        return mServerPort;
    }
    return 8082;
}
