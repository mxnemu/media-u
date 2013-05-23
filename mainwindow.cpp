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

void MainWindow::setPage(const QString& pageName)
{
    std::cout << "setpage " << pageName.toStdString() << std::endl;
    if (page) {
        delete page; // also removes from parent
    }
    page = pageFactory.pageForKey(pageName);
    if (page) {
        this->ui->verticalLayout->addWidget(page);
    }
}
