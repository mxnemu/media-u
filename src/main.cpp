#include "mainwindow.h"
#include <QApplication>
#include <QStatusBar>
#include <QDir>
#include "config.h"
#include "server.h"
#include "library.h"
#include "malclient.h"
#include "mplayer.h"
#include "omxplayer.h"
#include "metadataparseravconv.h"
#include "thumbnailcreatoravconv.h"
#include "transmissionclient.h"
#include "nyaarss.h"

#include <string.h>
#include <curl/curl.h>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    curl_global_init(CURL_GLOBAL_SSL);

    // TODO move argument parsing partly back here, since it calls exit(0)
    Config config(argc, argv);

    Library library(config.libraryPath());
    library.readAll();

    MainWindow w(library);
    if (!config.getNoGui()) {
        if (config.getFullScreen()) {
            w.showFullScreen();
        } else {
            w.show();
        }
    }

    VideoPlayer* player;
    if (config.omxPlayerIsInstalled()) {
        player = new Omxplayer(library);
    } else {
        player = new Mplayer(library, config.getMplayerConfigConstRef());
    }

    MetaDataParserAvconv metaDataParser;
    ThumbnailCreatorAvconv thumbnailCreator;
    player->setMetaDataParser(&metaDataParser);
    player->setThumbnailCreator(&thumbnailCreator);
    library.setMetaDataParser(&metaDataParser);

    // TODO move into the thread
    TransmissionClient transmission;
    NyaaRss::Client nyaaClient(transmission, library);
    TorrentRss::Thread rssThread(nyaaClient);
    rssThread.start(QThread::LowestPriority);


    QDir publicDir = QDir::current();
    publicDir.cd("public");
    Server s(publicDir.path(), w, library, player);
    int port = s.start(config.serverPort());

    w.statusBar()->showMessage(QString("Launched on port %1").arg(port));
    w.setPage("MainPage");

    library.initMalClient(config.malConfigFilePath());
    library.startSearch();

    int returnCode = a.exec();
    library.write(); // write before exit
    return returnCode;
}
