#include "mainwindow.h"
#include <QApplication>
#include "server.h"
#include <QStatusBar>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Server s;
    int port = s.start();

    w.statusBar()->showMessage(QString("Launched on port %1").arg(port));
    
    return a.exec();
}
