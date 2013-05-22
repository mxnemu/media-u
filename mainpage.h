#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include <library.h>

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainPage(Library& library, QWidget *parent = 0);
    ~MainPage();
    
private:
    Ui::MainPage *ui;
};

#endif // MAINPAGE_H
