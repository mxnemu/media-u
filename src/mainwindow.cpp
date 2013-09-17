#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QDropEvent>
#include <QMimeData>

MainWindow::MainWindow(Library &library, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    library(library),
    pageFactory(library, this)
{
    ui->setupUi(this);
    page = NULL;
    this->setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPage(const QString& pageName, const QString& initString)
{
    std::cout << "setpage " << pageName.toStdString() << std::endl;
    if (page) {
        page->deleteLater();
       // delete page; // also removes from parent
    }
    page = pageFactory.pageForKey(pageName);
    if (page) {
        this->ui->verticalLayout->addWidget(page);
        this->pageId = pageName;
        page->initFromQuery(initString);
    } else {
        this->page = NULL;
    }
}

void MainWindow::setPage(const QString &pageName)
{
    setPage(pageName, QString());
}

QString MainWindow::activePageId() {
    return pageId;
}

Page *MainWindow::activePage() {
    return page;
}

MainBackgroundWidget *MainWindow::getCentralWidget() {
    return ui->centralWidget;
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event) {
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event) {
    if (!event->mimeData()) {
        event->dropAction();
        return;
    }
    const QList<QUrl> urls = event->mimeData()->urls();
    bool addedDirectory = false;
    for (int i=0; i < urls.length(); ++i) {

        const QUrl& url = urls.at(i);
        if (QDir(url.path()).exists()) {
            library.addSearchDirectory(SearchDirectory(url.path()));
            addedDirectory = true;
        }
    }
    if (addedDirectory) {
        event->accept();
        library.startSearch();
        qDebug() << "search dirs added!";
    } else {
        event->accept();
    }
}
