#include "routerwindow.h"
#include "memorywindow.h"
#include "injectorwindow.h"
#include "global.h"

#include "ui_routerwindow.h"
#include "qdebug.h"

RouterWindow::RouterWindow(quint8 subType, quint8 subTypeUp, quint8 routerNumber, quint8 routerLevel, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RouterWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("Router %1, Level %2").arg(routerNumber).arg(routerLevel));

    this->subType = subType;
    this->subTypeUp = subTypeUp;
    this->routerNumber = routerNumber;
    this->routerLevel = routerLevel;
    qDebug() << "*RouterWindow* router window created subType " << subType << "subTypeUp " << subTypeUp;
}

RouterWindow::~RouterWindow()
{
    delete ui;
    qDebug() << "router window destroyed";
}

bool RouterWindow::link(struct DataOnLink *info)
{
    bool returnValue;

    qDebug() <<"RouterWindow::link  router level up down" << routerNumber << routerLevel << info->UpInfo << info->DownInfo;
    ui->LinkHeaderUplabel->setText(QString::number(info->UpInfo,2));
    ui->LinkHeaderDownlabel->setText(QString::number(info->DownInfo,2));
    ui->Modelabel->setNum(info->Mode);

    returnValue = false;

    if (info->UpInfo > 0) {
        ui->Uplabel->setText("Up");
        qDebug() << "router number" << routerNumber << routerLevel << "up";
        qDebug() << "info->UpInfo" << info->UpInfo;
        info->UpInfo >>= 1;
        qDebug() << "info->UpInfo >> 1" << info->UpInfo;
        qDebug() << "subTypeUp" << subTypeUp;
        switch (subTypeUp){
            case 0: if (Up != nullptr){
                        qDebug() << "Up";
                        returnValue =  Up->link(info);
                    } else
                        qDebug() << "Up = nullptr";
                    break;
            case 1: if (UpInj != nullptr){
                        qDebug() << "UpInjector";
                        returnValue = UpInj->link(info);
                    } else
                        qDebug() << "UpInj = nullptr";
                    break;
            default: qDebug() << "default";
        }
        ui->Uplabel->setText("");
        qDebug() << "returnValue" << returnValue;
        return returnValue;
    } else {
        qDebug() << "router number down switch DownInfo" << routerNumber << routerLevel << info->DownInfo << "subType" << subType;
        qDebug() << "info->DownInfo & 7: " << (info->DownInfo & 7);
        switch(info->DownInfo & 7){
            case 0: ui->Down0label->setText("Down");
                    qDebug() << "router-number" << routerNumber << routerLevel << "down0" << " subType " << subType;
                    info->DownInfo >>= 3;
                    switch (subType){
                        case 0: if (Down0 != nullptr)
                                    returnValue = Down0->link(info);
                                break;
                        case 1: if (Down0Mem != nullptr)
                                    returnValue = Down0Mem->link(info);
                                break;
                    }
                    ui->Down0label->setText("");
                    return returnValue;

            case 1: ui->Down1label->setText("Down");
                    qDebug() << "router number" << routerNumber << routerLevel << "down1" << " subType " << subType;
                    info->DownInfo >>= 3;
                    switch (subType){
                        case 0: if (Down1 != nullptr)
                                    returnValue = Down1->link(info);
                                break;
                        case 1: if (Down1Mem != nullptr)
                                    returnValue = Down1Mem->link(info);
                                break;
                    }
                    ui->Down1label->setText("");
                    return returnValue;

            case 2: ui->Down2label->setText("Down");
                    qDebug() << "router number" << routerNumber << routerLevel << "down2" << " subType " << subType;
                    info->DownInfo >>= 3;
                    switch (subType){
                        case 0: if (Down2 != nullptr)
                                    returnValue = Down2->link(info);
                                break;
                        case 1: if (Down2Mem != nullptr)
                                    returnValue = Down2Mem->link(info);
                                break;
                    }
                    ui->Down2label->setText("");
                    return returnValue;

            case 3: ui->Down3label->setText("Down");
                    qDebug() << "router number" << routerNumber << routerLevel << "down3" << " subType " << subType;
                    info->DownInfo >>= 3;
                    switch (subType){
                        case 0: if (Down3 != nullptr)
                                    returnValue = Down3->link(info);
                                break;
                        case 1: if (Down3Mem != nullptr)
                                    returnValue = Down3Mem->link(info);
                                break;
                    }
                    ui->Down3label->setText("");
                    return returnValue;

            case 4: ui->Down4label->setText("Down");
                    qDebug() << "router number" << routerNumber << routerLevel << "down4" << " subType " << subType;
                    info->DownInfo >>= 3;
                    switch (subType){
                        case 0: if (Down4 != nullptr)
                                    returnValue = Down4->link(info);
                                break;
                        case 1: if (Down4Mem != nullptr)
                                    returnValue = Down4Mem->link(info);
                                break;
                    }
                    ui->Down4label->setText("");
                    return returnValue;

            case 5: ui->Down5label->setText("Down");
                    qDebug() << "router number" << routerNumber << routerLevel << "down5" << " subType " << subType;
                    info->DownInfo >>= 3;
                    switch (subType){
                        case 0: if (Down5 != nullptr)
                                    returnValue = Down5->link(info);
                                break;
                        case 1: if (Down5Mem != nullptr)
                                    returnValue = Down5Mem->link(info);
                                break;
                    }
                    ui->Down5label->setText("");
                    return returnValue;

            case 6: ui->Down6label->setText("Down");
                    qDebug() << "router number" << routerNumber << routerLevel << "down6" << " subType " << subType;
                    info->DownInfo >>= 3;
                    switch (subType){
                        case 0: if (Down6 != nullptr)
                                    returnValue = Down6->link(info);
                                break;
                        case 1: if (Down6Mem != nullptr)
                                    returnValue = Down6Mem->link(info);
                                break;
                    }
                    ui->Down6label->setText("");
                    return returnValue;

            case 7: ui->Down7label->setText("Down");
                    qDebug() << "router number" << routerNumber << routerLevel << "down7" << " subType " << subType;
                    info->DownInfo >>= 3;
                    switch (subType){
                        case 0: if (Down7 != nullptr)
                                    returnValue = Down7->link(info);
                                break;
                        case 1: if (Down7Mem != nullptr)
                                    returnValue = Down7Mem->link(info);
                                break;
                    }
                    ui->Down7label->setText("");
                    return returnValue;

            default:
                    qDebug() << "router number default case";
                    return returnValue;
        }
    }
}

void RouterWindow::setDownLink(quint8 linkNumber, RouterWindow *pointer)
{
    qDebug() << "*RouterWindow* setDownLink " << linkNumber;
    qDebug() << " Pointer to Router: " << pointer;
    switch(linkNumber){
        case 0: Down0 = pointer;
                break;
        case 1: Down1 = pointer;
            break;
        case 2: Down2 = pointer;
            break;
        case 3: Down3 = pointer;
            break;
        case 4: Down4 = pointer;
            break;
        case 5: Down5 = pointer;
            break;
        case 6: Down6 = pointer;
            break;
        case 7: Down7 = pointer;
            break;
    }
}

void RouterWindow::setDownLinkMem(quint8 linkNumber, MemoryWindow *pointer)
{
     qDebug() << "*RouterWindow* setDownLinkMem " << routerNumber << " level " << routerLevel << " link " << linkNumber << " Pointer to Memory:" << pointer;
     switch(linkNumber){
        case 0: Down0Mem = pointer;
                break;
        case 1: Down1Mem = pointer;
            break;
        case 2: Down2Mem = pointer;
            break;
        case 3: Down3Mem = pointer;
            break;
        case 4: Down4Mem = pointer;
            break;
        case 5: Down5Mem = pointer;
            break;
        case 6: Down6Mem = pointer;
            break;
        case 7: Down7Mem = pointer;
            break;
    }
}

void RouterWindow::setUpLink(RouterWindow *pointer)
{
    qDebug() << "*RouterWindow *setUpLink " << routerNumber << routerLevel << "Pointer: " << pointer;
    Up = pointer;
}

void RouterWindow::setUpLinkInj(InjectorWindow *pointer)
{
    qDebug() << "*RouterWindow* setUpLinkInj Pointer:" << pointer;
    UpInj = pointer;
}
