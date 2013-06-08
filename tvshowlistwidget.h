#ifndef GUITVSHOWLIST_H
#define GUITVSHOWLIST_H

#include <QWidget>
#include "tvshow.h"

namespace Ui {
class GuiTvShowList;
}

class TvShowListWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TvShowListWidget(QWidget *parent = 0);
    ~TvShowListWidget();

    void set(QList<TvShow *> showList, QString title = QString());
    void setFields(QStringList fields); // TODO impl for customization
private:
    Ui::GuiTvShowList *ui;
};

#endif // GUITVSHOWLIST_H
