#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "library.h"
#include "pagefactory.h"
#include "page.h"
#include "mainbackgroundwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(Library& library, QWidget *parent = 0);
    ~MainWindow();

    void setPage(const QString& pageName, const QString &initString);
    void setPage(const QString& pageName);
    QString activePageId();
    Page* getPage();

    MainBackgroundWidget* getCentralWidget();
    
    void dropEvent(QDropEvent *drop);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
private:
    Ui::MainWindow *ui;
    Library& library;
    PageFactory pageFactory;
    Page* page;
    QString pageId;
    MainWindow* mainwindow;
};

#endif // MAINWINDOW_H
