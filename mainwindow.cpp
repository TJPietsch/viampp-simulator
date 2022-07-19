#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "routerwindow.h"
#include "memorywindow.h"
#include "cpuwindow.h"
#include "injectorwindow.h"

const quint8 EMPTY = 0;
const quint8 ROUTER = 1;
const quint8 MEMORY = 2;
const quint8 CPU = 3;
const quint8 INJECTOR = 4;

bool global_run = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow::MainWindow";

    ui->setupUi(this);
//    scene = new myGraphicsScene(this);

//    ui->graphicsView->mySetScene(scene);
//    ui->graphicsView->setGridVisible(false);

    /** Note: if you used the QMainWindow, you must both setMouseTracking true in QMainWindow class and centralWidget */
    setMouseTracking(true);
//    ui->centralWidget->setMouseTracking(true);

//    QPen helperPen(QColor(100, 100, 100) , 1.5);
//    scene->addEllipse(-4.0,-4.0, 8.0, 8.0, helperPen);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createStatusBar()
{
    qDebug() << "MainWindow::createStatusBar";

    statusBar()->showMessage(tr("Ready"), 2000);
}

void MainWindow::on_actionOpen_Configuration_triggered()
{
    qDebug() << "MainWindow::on_actionOpen_Configuration_triggered";

    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        configStream.setDevice(&file);
        scene = new myGraphicsScene(this);
        ui->graphicsView->setScene(scene);
        ui->configNameLabel->setText(QString(fileName));

        injector = new InjectorWindow (0,this);
        qDebug() << "Injector Pointer: " << injector;
        injector_symbol = new draw(QColor(255,255,255,255), "Injector", INJECTOR, injector);
        injector_symbol->setPos(100,-100);
        scene->addItem(injector_symbol);

        qDebug() << "start read config";
        root_node = read_mpp_elements((mpp_element *) injector, QPoint(120,-80));

        injector->setLevel(root_node->mpp_router.routerLevel);
        injector->setDownLinkRouter((RouterWindow *)root_node->mpp_element_window_ptr);
        qDebug () << "Injector points to: " << (RouterWindow *) root_node->mpp_element_window_ptr;
        file.close();
    }
    statusBar()->showMessage(tr("File loaded"), 2000);
    configuration_loaded = true;

}

void MainWindow::on_globalRunButton_clicked()
{
    qDebug() << "MainWindow: global_run: " << global_run;

    if (configuration_loaded) {
        if (global_run) {
            ui->globalRunButton->setText("Start");
            global_run = false;
        } else {
            ui->globalRunButton->setText("Stop");
            global_run = true;
        }
    } else {
        QMessageBox::information(this, "Viampp", "Please load a configuration.", QMessageBox::Ok);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow:closeEvent";

    int result = QMessageBox::warning(this, "Viampp", "Do you want to terminate the Simulator?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::Yes){
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::on_actionQuit_triggered()
{
    qDebug() << "Viampp::on_actionQuit_triggered";

    QWidget::close();
}


struct mpp_element *MainWindow::read_mpp_elements(struct mpp_element *node, QPointF parent)
{
    qDebug() <<"*MainWindow::read_mpp_elements";

    struct mpp_element *element = nullptr;
    quint8 type;

    if (!configStream.atEnd()) {
        configStream >> type;
        switch (type) {
        case EMPTY: qDebug() << "+MainWindow+ read empty";
                    return nullptr;
        case ROUTER: {
            qDebug() << "+MainWindow+ read Router. Node:" << node;
            element = new mpp_element;
            element->Up = node;
            element->mpp_type = ROUTER;
            configStream >> element->description;
            configStream >> element->x;
            configStream >> element->y;
            configStream >> element->mpp_router.subType;
            configStream >> element->mpp_router.subTypeUp;
            configStream >> element->mpp_router.routerNumber;
            configStream >> element->mpp_router.routerLevel;

            qDebug() << "+MainWindow+ Create RouterWindow type description x y: " << type << element->description << element->x << element->y;
            element->mpp_element_window_ptr = new RouterWindow(element->mpp_router.subType,element->mpp_router.subTypeUp,element->mpp_router.routerNumber,element->mpp_router.routerLevel,this);

            qDebug() << "+MainWindow+ New Router Window, Pointer: " << element->mpp_element_window_ptr;
            if (element->mpp_router.subTypeUp == 1) {
//                qDebug() << "===============Set Routerto Injector=================" << element->Up;
                ((RouterWindow *)element->mpp_element_window_ptr)->setUpLinkInj((InjectorWindow *)element->Up);
            } else
                ((RouterWindow *)element->mpp_element_window_ptr)->setUpLink((RouterWindow *)element->Up);

            element->mpp_element_icon_ptr = new draw(QColor(255,255,255,255), element->description, element->mpp_type, element->mpp_element_window_ptr);
            element->mpp_element_icon_ptr->setPos(element->x, element->y);
            scene->addItem(element->mpp_element_icon_ptr);

            QPointF son(element->x + 20,element->y + 20);
            connectionLine = new connectElements(QColor(Qt::black),son, parent);
            scene->addItem(connectionLine);
            parent = son;

            for (int i = 0; i < 8; i++) {   // follow the down links
                element->mpp_router.Down[i] = read_mpp_elements(element,parent);
            }

            for (int i = 0; i < 8; i++) {   // set down links
                if (element->mpp_router.Down[i]) {
                    if (element->mpp_router.subType == 0)   // sybType = router
                        ((RouterWindow *)element->mpp_element_window_ptr)->setDownLink(0, (RouterWindow *)element->mpp_router.Down[i]->mpp_element_window_ptr);
                    else                                    // subType = memory
                        ((RouterWindow *)element->mpp_element_window_ptr)->setDownLinkMem(0, (MemoryWindow *)element->mpp_router.Down[i]->mpp_element_window_ptr);
                }
            }

            return element;
        }
        case MEMORY: {
            qDebug() << "+MainWindow+     read Memory. Node:" << node;
            element = new mpp_element;
            element->Up = node;
            element->mpp_type = MEMORY;
            configStream >> element->description;
            configStream >> element->x;
            configStream >> element->y;
            configStream >> element->mpp_memory.size;
            configStream >> element->mpp_memory.segment;

// qDebug() << "+MainWindow+     type description x y: " << type << element->description << element->x << element->y;

            element->mpp_element_window_ptr = new MemoryWindow(element->mpp_memory.size, element->mpp_memory.segment, (RouterWindow *)element->Up->mpp_element_window_ptr, this);
            qDebug() << "+MainWIndow+     New Memory Window:  Memory size, Memory segment, Memory Pointer, points to: " << element->mpp_memory.size << element->mpp_memory.segment<< element->mpp_element_window_ptr << (RouterWindow *)element->Up->mpp_element_window_ptr;
            element->mpp_element_icon_ptr = new draw(QColor(255,255,255,255), element->description, element->mpp_type, element->mpp_element_window_ptr);
            element->mpp_element_icon_ptr->setPos(element->x, element->y);
            scene->addItem(element->mpp_element_icon_ptr);

            QPointF son(element->x + 20,element->y + 25);
            connectionLine = new connectElements(QColor(Qt::black),son, parent);
            scene->addItem(connectionLine);
            parent = son;

            for (int i = 0; i < 4; i++) {       // 16 cpus later ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
               element->mpp_memory.Cpu[i] = read_mpp_elements(element,parent);
               if (element->mpp_memory.Cpu[i])
                   ((MemoryWindow *)element->mpp_element_window_ptr)->Cpu[i] = ((CPUWindow *)element->mpp_memory.Cpu[i]->mpp_element_window_ptr)->getCpuThreadPointer();
            }
            return element;
        }
        case CPU: {
            qDebug() << "+MainWindow+         read CPU. Node:" << node;
            element = new mpp_element;
            element->Up = node;
            element->mpp_type = CPU;
            configStream >> element->description;
            configStream >> element->x;
            configStream >> element->y;
            configStream >> element->mpp_cpu.cpu_number;

//  qDebug() << "+MainWIndow+         Create CPUWindow and Icon in scene: type description x y: " << type << element->description << element->x << element->y;
            element->mpp_element_window_ptr = new CPUWindow(element->mpp_cpu.cpu_number, (MemoryWindow *)element->Up->mpp_element_window_ptr, this);
            qDebug() << "+MainWIndow+         New CPUWindow: " << "CPU Pointer: " << element->mpp_element_window_ptr << "points to: "<< element->Up->mpp_element_window_ptr;
            element->mpp_element_icon_ptr = new draw(QColor(255,255,255,255), element->description, element->mpp_type, element->mpp_element_window_ptr);
            element->mpp_element_icon_ptr->setPos(element->x, element->y);
            scene->addItem(element->mpp_element_icon_ptr);

            QPointF son(element->x + 15,element->y);
            connectionLine = new connectElements(QColor(Qt::black),son, parent);
            scene->addItem(connectionLine);
            parent = son;

            return element;
        }
        default: qDebug() << "+MainWindow+ read unknown";
            return nullptr;
        }
    }
}

void MainWindow::delete_mpp_elements(struct mpp_element *node)
{
    qDebug() << "MainWindow::delete_mpp_elements";

    switch (node->mpp_type) {
    case ROUTER: {
        qDebug() << "+MainWindow+ read Router. Node:" << node;
        for (int i = 0; i < 8; i++) {
            if (node->mpp_router.Down[i])
                delete_mpp_elements(node->mpp_router.Down[i]);
        }
        break;
    }
    case MEMORY: {
        qDebug() << "+MainWindow+     read Memory. Node:" << node;
        for (int i = 0; i < 4; i++) {       // 16 cpus later ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            if (node->mpp_memory.Cpu[i])
                delete_mpp_elements(node->mpp_memory.Cpu[i]);
        }
        break;
    }
    }

    delete node->mpp_element_window_ptr;
    delete node;
    return;

}

