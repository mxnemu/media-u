#include "config.h"
#include <QFile>
#include <QDir>
#include <iostream>

Config::Config(QString initPath)
{
    this->init(initPath);
    mServerPort = -1;
    initialized = false;
}

bool Config::init(QString path) {
    QDir dir;
    if (path.isNull() || path.isEmpty()) {
        dir = QDir::home();
    } else {
        dir = QDir(path);
    }

    if (!dir.cd(".mediaU")) {
        if (!dir.mkdir(dir.absoluteFilePath(".mediaU"))) {
            // TODO global error quit with dialogue message
            // home not writeable
        }
        dir.cd(".mediaU");
    }

    QFile configFile(dir.absoluteFilePath("config.json"));
    if (configFile.exists() && configFile.open(QFile::ReadOnly) && configFile.size() > 0) {
        return this->parse(configFile.readAll());
    } else {
        return this->createNewConfig(dir.absoluteFilePath("config.json"));
    }
}

bool Config::parse(const QByteArray& jsonData)
{
    // TODO parse json
    QDir home = QDir::home();
    mLibraryPath = home.absoluteFilePath(".mediaU/library/");
    mServerPort = 8082;
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

QString Config::libraryPath() {
    if (initialized) {
        return this->mLibraryPath;
    }

    QDir dir = QDir::home();
    return dir.absoluteFilePath(".mediaU/library");
}

int Config::serverPort() {
    if (initialized && mServerPort > 0) {
        return mServerPort;
    }
    return 8082;
}
