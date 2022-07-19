#include "global.h"
#include "injectorwindow.h"
#include "routerwindow.h"
#include "memorywindow.h"

#include "ui_injectorwindow.h"
#include "qdebug.h"

InjectorWindow::InjectorWindow(quint8 subType, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InjectorWindow)
{
    ui->setupUi(this);
    this->subType = subType;
    this->level = 7 - level;

    setWindowTitle("Injector");

    ui->StatusMessagePlainTextEdit->insertPlainText ("\tWelcome to VIAMPP\n(Vienna Integrated Asynchronous Massive Parallel Processing)\n\n");
    ui->StatusMessagePlainTextEdit->insertPlainText ("ready!\n\n");

    DataLink = new DataOnLink;
}

InjectorWindow::~InjectorWindow()
{
    delete ui;

    delete DataLink;
}

quint32 InjectorWindow::CalcAddress(quint64 address)
{
    QString down_info_str ="";
    QString down_str = QString("%0").arg((DataLink->Address >> 40),24,2,QChar('0'));
    qDebug() << "+InjectorWindow:CalcAddress+ Level: " << level;
    if (level >= 7){
        down_info_str.prepend(down_str.at(2));
        down_info_str.prepend(down_str.at(1));
        down_info_str.prepend(down_str.at(0));
        qDebug() << " level 7 down_info_str: " << down_info_str;
    }
    if (level >= 6){
        down_info_str.prepend(down_str.at(5));
        down_info_str.prepend(down_str.at(4));
        down_info_str.prepend(down_str.at(3));
        qDebug() << " level 6 down_info_str: " << down_info_str;
    }
    if (level >= 5){
        down_info_str.prepend(down_str.at(8));
        down_info_str.prepend(down_str.at(7));
        down_info_str.prepend(down_str.at(6));
        qDebug() << " level 5 down_info_str: " << down_info_str;
    }
    if (level >= 4){
        down_info_str.prepend(down_str.at(11));
        down_info_str.prepend(down_str.at(10));
        down_info_str.prepend(down_str.at(9));
        qDebug() << " level 4 down_info_str: " << down_info_str;
    }
    if (level >= 3){
        down_info_str.prepend(down_str.at(14));
        down_info_str.prepend(down_str.at(13));
        down_info_str.prepend(down_str.at(12));
        qDebug() << " level 3 down_info_str: " << down_info_str;
    }
    if (level >= 2){
        down_info_str.prepend(down_str.at(17));
        down_info_str.prepend(down_str.at(16));
        down_info_str.prepend(down_str.at(15));
        qDebug() << " level 2 down_info_str: " << down_info_str;
    }
    if (level >= 1){
        down_info_str.prepend(down_str.at(20));
        down_info_str.prepend(down_str.at(19));
        down_info_str.prepend(down_str.at(18));
        qDebug() << " level 1 down_info_str: " << down_info_str;
    }
    if (level >= 0){
        down_info_str.prepend(down_str.at(23));
        down_info_str.prepend(down_str.at(22));
        down_info_str.prepend(down_str.at(21));
        qDebug() << " level 0 down_info_str: " << down_info_str;
    }
    qDebug() << "+InjectorWindow:CalcAddress+ down_str: " <<  down_str  << "down_info_str"<< down_info_str;
    return down_info_str.toUInt(&ok, 2);
}

void InjectorWindow::on_ReadPushButtonMemoryTab_clicked()
{
    qDebug() << "Read Button pressed";
    DataLink->Address = ui->AddressLineEditMemoryTab->text().toLongLong(&okAddress,10);

   if (okAddress){
        DataLink->UpInfo = 0;
        DataLink->DownInfo = CalcAddress(DataLink->Address);
        DataLink->Mode = READ;
        qDebug() << "Read sending data down the link";
        if (subType == 0){
            qDebug() << "sendig to router";
            ok = DownLinkRouter->link(DataLink);
        } else {
            qDebug() << "sendig to memory";
            ok = DownLinkMemory->link(DataLink);
        }
        if (!ok){
            qDebug() << "Error: Address not existing\n";
            ui->StatusMessagePlainTextEdit->insertPlainText("Error: Address not existing\n");
        } else {
            ui->DataLineEditMemoryTab->setText(QString("%0").arg(QString().number(DataLink->word.data,10),20,QChar('0')));
        }
    } else {
        qDebug() << "address invalid format";
        ui->StatusMessagePlainTextEdit->insertPlainText("Error: Invalid address format!\n");
    }
}

void InjectorWindow::on_WritePushButtonMemoryTab_clicked()
{
    qDebug() << "Write Button pressed";
    DataLink->Address = ui->AddressLineEditMemoryTab->text().toLongLong(&okAddress,10);
    DataLink->word.data = ui->DataLineEditMemoryTab->text().toLongLong(&okData, 10);
    DataLink->word.field = 0;
    if (okAddress & okData){
        DataLink->UpInfo = 0;
        DataLink->DownInfo = CalcAddress(DataLink->Address);
        DataLink->Mode = WRITE;
        qDebug() << "Write sending data down the link";
        if (subType == 0){
            qDebug() << "sendig to router";
            ok = DownLinkRouter->link(DataLink);
        } else {
            qDebug() << "sendig to memory";
            ok = DownLinkMemory->link(DataLink);
        }
        if (!ok){
            qDebug() << "Error: Address not existing\n";
            ui->StatusMessagePlainTextEdit->insertPlainText("Error: Address not existing\n");
        }
    } else {
        qDebug() << "data or address invalid format";
        ui->StatusMessagePlainTextEdit->insertPlainText("Error: Invalid address or data format!\n");
    }
}


bool InjectorWindow::link(struct DataOnLink *DataLink)
{
    if (DataLink->Mode == INJ){
        DataLink->UpInfo = 0;
        qDebug() << "Signal received";
        ui->StatusMessagePlainTextEdit->insertPlainText("Signal received from xxx\n");
    }
    return true;
}

void InjectorWindow::setDownLinkRouter (RouterWindow *pointer)
{
    DownLinkRouter = pointer;
}

void InjectorWindow::setDownLinkMem (MemoryWindow *pointer)
{
    DownLinkMemory = pointer;
}

void InjectorWindow::setLevel (quint8 level)
{
    this->level = 7 - level;
}

void InjectorWindow::on_SendPushButtonSignalTab_clicked()
{
    bool ok, okSegment, okAddress, okData;
    qDebug() << "Send Button pressed!";
    DataLink->Address = ui->SegmentSpinBoxSignalTab->text().toLongLong(&okSegment,10) * 1099511627776 + ui->CpuSpinBoxSignalTab->text().toLongLong(&okAddress,10);
    DataLink->word.data = ui->SignaLineEditSignalTab->text(). toLongLong(&okData,2);
    if (okSegment && okAddress && okData){
        qDebug() << "DataLink->Address: " << DataLink->Address << "Data: "  << DataLink->word.data;
        DataLink->UpInfo = 0;
        DataLink->DownInfo = CalcAddress(DataLink->Address);
        DataLink->Mode = SIG;
        qDebug() << "Signal sending data down the link";
        if (subType == 0){
            qDebug() << "sendig to router";
            ok = DownLinkRouter->link(DataLink);
        } else {
            qDebug() << "sendig to memory";
            ok = DownLinkMemory->link(DataLink);
        }
        if (!ok){
            qDebug() << "Error: Segment or CPU not existing\n";
            ui->StatusMessagePlainTextEdit->insertPlainText("Error: Segment or CPU not existing\n");
        } else {
            qDebug() << "Signal sent";
            ui->StatusMessagePlainTextEdit->insertPlainText(QString("Signal sent to Segment %1, CPU %2\n").arg(ui->SegmentSpinBoxSignalTab->text()).arg(ui->CpuSpinBoxSignalTab->text()));
        }
    }
}

void InjectorWindow::on_OpenPushButtonSoftwareTab_clicked()
{
    qDebug() << "Open Button pressed";
    QString line;

    QTextStream textStream;
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()){
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        textStream.setDevice(&file);
        while (textStream.readLineInto(&line)){
            myProgram.append(line);
        }
            ui->SoftwareListWidgetSoftwareTab->addItem(QString("->%1 loaded\n").arg(fileName));
    }
}

void InjectorWindow::on_WritePushButtonSoftwareTab_clicked()
{
    qDebug() << "Write Button pressed";
    bool ok, okData, okAddress;
    quint8 field = 128;
    quint64 memoryLocation = 0;
    quint32 line = 0;
    QString progLine;

    quint64 j = ui->AddressLineEditSoftwareTab->text().toLongLong(&okAddress,10);
    if (okAddress) memoryLocation = j;
    qDebug() << "Program size " << myProgram.size() << " lines";
    for (line = 0; line < myProgram.size(); line++){
        progLine = myProgram.at(line);
        qDebug() << "line " << line << " data " << progLine;
        if (progLine.contains(".")){
            qDebug() << "we have declaration";
            if (progLine.contains("data")) field = 0;
            if (progLine.contains("code")) field = 128;
            if (progLine.contains("org")) {
                progLine.remove(0,5);
                memoryLocation = progLine.toLongLong(&ok, 10);
                qDebug() << ".org " << memoryLocation;
            }
        } else {
            /* write data to memory */
            DataLink->Address = memoryLocation++;
            qDebug() << "DataLink->Address: " << DataLink->Address;
            DataLink->word.data = progLine.toLongLong(&okData, 2);
            DataLink->word.field = field;
            if (okData){
                DataLink->UpInfo = 0;
                DataLink->DownInfo = CalcAddress(DataLink->Address);
                DataLink->Mode = WRITE;
                qDebug() << "Write sending data down the link";
                if (subType == 0){
                    qDebug() << "sendig to router";
                    ok = DownLinkRouter->link(DataLink);
                } else {
                    qDebug() << "sendig to memory";
                    ok = DownLinkMemory->link(DataLink);
                }
                if (!ok){
                    qDebug() << "Error: Address not existing\n";
                    ui->StatusMessagePlainTextEdit->insertPlainText("Error: Address not existing\n");
                }
            } else {
                qDebug() << "data or address invalid format";
                ui->StatusMessagePlainTextEdit->insertPlainText("Error: Invalid address or data format!\n");
            }
        } // end if declaration
    } // end for
    ui->StatusMessagePlainTextEdit->insertPlainText("Program written\n");

}
