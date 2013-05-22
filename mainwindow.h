#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "library.h"
#include "pagefactory.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(Library& library, QWidget *parent = 0);
    ~MainWindow();

    void setPage(const QString& pageName);
    
private:
    Ui::MainWindow *ui;
    Library& library;
    PageFactory pageFactory;
    QWidget* page;
};

#endif // MAINWINDOW_H
