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

bool Config::parse(const QString& jsonData)
{
    // TODO parse json
    QDir home = QDir::home();
    std::string nwlibraryPath = home.absoluteFilePath(".mediaU/library/").toStdString();
    mServerPort = 8082;
    initialized = true;



    istringstream input(jsonData.toStdString());
    nw::JsonReader jr(input);
    jr.describe("port", mServerPort);
    jr.push("library");
    jr.describe("path", nwlibraryPath);
    jr.pop();
    jr.close();

    mLibraryPath = QString(nwlibraryPath.data());

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
