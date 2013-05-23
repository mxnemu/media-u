#include "mainwindow.h"
#include <QApplication>
#include <QStatusBar>
#include <QDir>
#include "config.h"
#include "server.h"
#include "library.h"

#include <string.h>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    QString configPath;

    for (int i=0; i < argc; ++i) {

        if (strcmp(argv[i], "--configdir")) {
            configPath = (QString(argv[i]));

            QDir dir(configPath);
            if (!dir.exists() && !dir.mkdir(QString(argv[i]))) {
                configPath = QString();
            }
        }
    }

    Config config(configPath);

    Library library(config.libraryPath());
    MainWindow w(library);
    w.show();

    QDir publicDir = QDir::current();
    publicDir.cd("public");
    Server s(publicDir.path(), w);
    int port = s.start(config.serverPort());


    w.statusBar()->showMessage(QString("Launched on port %1").arg(port));
    w.setPage("MainPage");
    
    return a.exec();
}
