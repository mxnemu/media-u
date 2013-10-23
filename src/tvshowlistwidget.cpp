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
        this->add(show, false);
    }
    this->sortList();

    if (!title.isNull()) {
        this->ui->titleLabel->setText(title);
    }
}

void TvShowListWidget::add(const TvShow* show, bool sort) {
    QStringList strings;
    strings.append(show->name());
    strings.append(QString("%1/%2/%3").arg(
        QString::number(show->episodeList().numberOfWatchedEpisodes()),
        QString::number(show->episodeList().numberOfEpisodes()),
        QString::number(show->getTotalEpisodes())
    ));
    QTreeWidgetItem* item = new QTreeWidgetItem(strings);
    this->ui->treeWidget->addTopLevelItem(item);
    if (sort) {
        this->sortList();
    }
}

void TvShowListWidget::sortList() {
    this->ui->treeWidget->resizeColumnToContents(0);
    this->ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);
}
