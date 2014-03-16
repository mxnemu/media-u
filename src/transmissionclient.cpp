#include "transmissionclient.h"
#include "systemutils.h"
#include <QDir>
#include <QDebug>

TransmissionClient::TransmissionClient(QObject *parent) :
    TorrentClient(parent),
    process(NULL)
{
    this->findCommand();
}

void TransmissionClient::newProcess() {
    if (process) {
        delete process;
    }
    process = new QProcess(NULL);
    process->setWorkingDirectory(QDir::temp().absolutePath());
}

bool TransmissionClient::addTorrent(QString filePath) {
    if (command.isNull()) {
        return false;
    }
    if (!process || process->state() == QProcess::Running) {
        newProcess();
    }

    process->start(command, QStringList() << filePath);
    process->waitForStarted();
    //process.waitForFinished();
    return true;
}

// TODO properly control the process
void TransmissionClient::findCommand() {
    if (SystemUtils::commandExists("transmission-cli")) {
        command = "transmission-cli";
    } else if (SystemUtils::commandExists("transmission-gtk")) {
        command = "transmission-gtk";
    } else if (SystemUtils::commandExists("transmission-qt")) {
        command = "transmission-qt";
    } else if (SystemUtils::commandExists("transmission-deamon")) {
        command = "transmission-deamon";
    } else {
        qDebug() << "transmission installation not found";
    }
}
