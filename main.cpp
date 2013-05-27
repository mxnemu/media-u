#include "mainwindow.h"
#include <QApplication>
#include <QStatusBar>
#include <QDir>
#include "config.h"
#include "server.h"
#include "library.h"
#include "directoryscanner.h"
#include "tvshowscanner.h"
#include "malclient.h"

#include <string.h>
#include <curl/curl.h>

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

    // debug scanner
    DirectoryScanner scanner(library);
    scanner.addScanner(new TvShowScanner(library));
    scanner.scan("/mnt/fields1/torrents/");
    scanner.scan("/media/nehmulos/INTENSO/anime");


    curl_global_init(CURL_GLOBAL_SSL);
    MalClient c;
    c.setCredentials(QString("notmy"), QString("realpassword"));
    
    return a.exec();
}
