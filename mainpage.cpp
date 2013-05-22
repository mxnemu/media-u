#include "mainpage.h"
#include "ui_mainpage.h"

MainPage::MainPage(Library& library, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);

    QString backgroundPath = library.randomWallpaperPath();
    this->setStyleSheet(QString("background-image: url(%1)").arg(backgroundPath));
}

MainPage::~MainPage()
{
    delete ui;
}
