#include "tvshowlistwidget.h"
#include "ui_tvshowlistwidget.h"

TvShowListWidget::TvShowListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GuiTvShowList)
{
    ui->setupUi(this);
}

TvShowListWidget::~TvShowListWidget()
{
    delete ui;
}

void TvShowListWidget::set(QList<TvShow*> showList, QString title) {
    this->ui->treeWidget->clear();
    for (int i=0; i < showList.length(); ++i) {
        const TvShow* show = showList.at(i);
        QStringList strings;
        strings.append(show->name());
        strings.append(QString("%1/%2/%3").arg(
            QString::number(show->getWatchedEpisodes()),
            QString::number(show->episodesDownloaded()),
            QString::number(show->getTotalEpisodes())
        ));
        QTreeWidgetItem* item = new QTreeWidgetItem(strings);
        this->ui->treeWidget->addTopLevelItem(item);
    }
    if (!title.isNull()) {
        this->ui->titleLabel->setText(title);
    }
    this->ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);
    this->ui->treeWidget->resizeColumnToContents(0);
}
