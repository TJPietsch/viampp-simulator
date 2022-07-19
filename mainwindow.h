#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "mygraphicsscene.h"
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QLabel>
#include <QSettings>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDataStream>
#include <QListWidget>
#include <QInputDialog>
//#include <QPrinter>
//#include <QPrintDialog>
//#include <QPrintPreviewDialog>

#include "global.h"
#include "draw.h"

class RouterWindow;
class MemoryWindow;
class CPUWindow;
class InjectorWindow;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_globalRunButton_clicked();
    void on_actionQuit_triggered();
    void on_actionOpen_Configuration_triggered();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    void createStatusBar();
    struct mpp_element *read_mpp_elements(struct mpp_element *node, QPointF parent);
    void delete_mpp_elements(struct mpp_element *node);

    myGraphicsScene *scene;

    bool configuration_loaded = false;

    QDataStream configStream;
    struct mpp_element *root_node = nullptr;
    connectElements *connectionLine;
    InjectorWindow *injector;
    QGraphicsItem *injector_symbol;
};
#endif // MAINWINDOW_H
