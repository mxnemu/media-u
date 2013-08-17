#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(Library &library, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    library(library),
    pageFactory(library)
{
    ui->setupUi(this);
    page = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPage(const QString& pageName, const QString& initString)
{
    std::cout << "setpage " << pageName.toStdString() << std::endl;
    if (page) {
        delete page; // also removes from parent
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
