#ifndef ROUTERWINDOW_H
#define ROUTERWINDOW_H

#include <QMainWindow>
#include "global.h"
//#include "memorywindow.h"
//#include "injectorwindow.h"

class MemoryWindow;
class InjectorWindow;

namespace Ui {
class RouterWindow;
}

class RouterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RouterWindow(quint8 subType, quint8 subTypeUp, quint8 routerNumber, quint8 routerLevel, QWidget *parent = nullptr);
    ~RouterWindow();

    bool link(struct DataOnLink *info);
    void setDownLink(quint8 linkNumber, RouterWindow *pointer);
    void setDownLinkMem(quint8 linkNumber, MemoryWindow *pointer);
    void setUpLink(RouterWindow *pointer);
    void setUpLinkInj(InjectorWindow *pointer);

private:
    Ui::RouterWindow *ui;

    quint8 subType, subTypeUp;
    quint8 routerNumber, routerLevel;

    RouterWindow *Up;
    RouterWindow *Down0, *Down1, *Down2, *Down3, *Down4, *Down5, *Down6, *Down7;

    InjectorWindow *UpInj;
    MemoryWindow *Down0Mem, *Down1Mem, *Down2Mem, *Down3Mem, *Down4Mem, *Down5Mem, *Down6Mem, *Down7Mem;
};

#endif // ROUTERWINDOW_H
