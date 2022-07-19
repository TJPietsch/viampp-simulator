#ifndef INJECTORWINDOW_H
#define INJECTORWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QMessageBox>
#include "global.h"

class RouterWindow;
class MemoryWindow;

namespace Ui {
class InjectorWindow;
}

class InjectorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit InjectorWindow(quint8 subtype, QWidget *parent = nullptr);
    ~InjectorWindow();

    bool link(struct DataOnLink *DataLink);
    void setDownLinkRouter (RouterWindow *pointer);
    void setDownLinkMem(MemoryWindow *pointer);
    void setLevel(quint8 level);


private slots:
    void on_ReadPushButtonMemoryTab_clicked();
    void on_WritePushButtonMemoryTab_clicked();

    void on_SendPushButtonSignalTab_clicked();

    void on_OpenPushButtonSoftwareTab_clicked();
    void on_WritePushButtonSoftwareTab_clicked();


private:
    Ui::InjectorWindow *ui;

    DataOnLink *DataLink;

    bool ok, okAddress, okData;
    quint8 level;

    quint8 subType;
    RouterWindow *DownLinkRouter;
    MemoryWindow *DownLinkMemory;
    QStringList myProgram;

    quint32 CalcAddress (quint64 address);
};

#endif // INJECTORWINDOW_H
