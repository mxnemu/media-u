#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::setPage(const QString& pageName)
{
    if (page) {
        delete page; // also removes from parent
    }
    page = pageFactory.pageForKey(pageName);
    if (page) {
        this->ui->verticalLayout->addWidget(page);
    }
}
