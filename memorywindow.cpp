#include "global.h"
#include "memorywindow.h"
#include "routerwindow.h"

#include "ui_memorywindow.h"

#include <QAbstractButton>

MemoryWindow::MemoryWindow(quint64 size, quint64 segment, RouterWindow *upLink, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MemoryWindow)
{
    ui->setupUi(this);

    bool ok;

    this->size = size;
    this->segment = segment;
    start = segment * 1099511627776;

//    qDebug() << "MemoryWindow constructor segment" << segment;
    this->upLink = upLink;

    end = start + size - 1;

    /* Setup Memory Segment and Initialize */
    shard = new MemoryWord[size]();
    qDebug() << "*MemoryWindow* " << size << " words of memory allocated for segment " << segment << " starting at " << start;

    setWindowTitle(QString("Memory Segment %1").arg(segment));

    /* Prepare source up info */

    source_str = QString("%1").arg(segment,24,2,QChar('0'));
    src_x_str = "";
    src_x_str.append(source_str.at(2));
    src_x_str.append(source_str.at(5));
    src_x_str.append(source_str.at(8));
    src_x_str.append(source_str.at(11));
    src_x_str.append(source_str.at(14));
    src_x_str.append(source_str.at(17));
    src_x_str.append(source_str.at(20));
    src_x_str.append(source_str.at(23));
    src_x = src_x_str.toUInt(&ok,2);
//    qDebug() << "src_x_str, srx_x" << src_x_str << src_x;
    src_y_str = "";
    src_y_str.append(source_str.at(1));
    src_y_str.append(source_str.at(4));
    src_y_str.append(source_str.at(7));
    src_y_str.append(source_str.at(10));
    src_y_str.append(source_str.at(13));
    src_y_str.append(source_str.at(16));
    src_y_str.append(source_str.at(19));
    src_y_str.append(source_str.at(22));
    src_y = src_y_str.toUInt(&ok,2);
//    qDebug() << "src_y_str, src_y" << src_y_str << src_y;
    src_z_str = "";
    src_z_str.append(source_str.at(0));
    src_z_str.append(source_str.at(3));
    src_z_str.append(source_str.at(6));
    src_z_str.append(source_str.at(9));
    src_z_str.append(source_str.at(12));
    src_z_str.append(source_str.at(15));
    src_z_str.append(source_str.at(18));
    src_z_str.append(source_str.at(21));
    src_z = src_z_str.toUInt(&ok,2);
//    qDebug() << "src_z_str, src_z" << src_z_str << src_z;

    /* generate Link Element */
    DataLink = new DataOnLink;

    /* Setup Memory Model */
    memModel = new MemoryModel(start, size, shard, this);
    ui->MemStart->setText(QString("%1").arg(start,8,10,QChar('0')));
    ui->MemSize->setText(QString("%1").arg(size,8,10,QChar('0')));
    ui->mem_tableView->setModel(memModel);
    ui->mem_tableView->resizeColumnsToContents();
    ui->displayTypeComboBox->addItem("hex");
    ui->displayTypeComboBox->addItem("dec");
    ui->displayTypeComboBox->addItem("bin");

    QAbstractButton *button = ui->mem_tableView->findChild<QAbstractButton *>();
    if(button){
        QVBoxLayout *layout = new QVBoxLayout(button);
        layout->setContentsMargins(0, 0, 0, 0);
        QLabel *label = new QLabel("Address");
        label->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(label);
    }

    ui->mem_tableView->show();
}

MemoryWindow::~MemoryWindow()
{
    delete ui;

    delete[] shard;
    qDebug() << "Memory for segment " << segment << " destroyed!!!";
}

MemoryModel::MemoryModel(quint64 start, quint64 size, MemoryWord *shard, QObject *parent) :
    QAbstractTableModel(parent)
{
    this->size = size;
    this->start = start;
    this->shard = shard;
}

int MemoryModel::rowCount(const QModelIndex & /*parent*/) const
{
    return size;
}

int MemoryModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}

QVariant MemoryModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
   if (role == Qt::DisplayRole){
       if (index.column() == 0){
           switch(displayType){
               case 0: return QString("0x%1").arg(shard[index.row()].data,16,16,QChar('0'));
               case 1: return QString("%1").arg(shard[index.row()].data,20,10,QChar('0'));
               case 2: return QString("0b%1").arg(shard[index.row()].data,64,2,QChar('0'));
           }
       }
       if (index.column() == 1) return this->dissasm(shard[index.row()]);
   }
   return QVariant();
}

QVariant MemoryModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        switch (section) {
            case 0:
                return QString("Data");
            case 1:
                return QString("Instruction / Data Type");
        }

    if (role == Qt::DisplayRole && orientation == Qt::Vertical)
        return QString("0x%1").arg(start + section,16,16,QChar('0'));

    return QVariant();
}

bool MemoryModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    qDebug() << "setData";
    emit dataChanged(index,index);
    return true;
}

void MemoryModel::setDisplayType(quint8 type)
{
    qDebug() << "setDisplayType";
    displayType = type;
    this->updateView(0);
}

void MemoryModel::updateMyView(quint64 cell, quint64 data)
{
    QModelIndex topLeft = createIndex(cell,0);
    qDebug() << "updateMyView topLeft: " << topLeft;
    this->setData(topLeft, data, Qt::DisplayRole);
}

void MemoryModel::updateView(quint64 cell)
{
    QModelIndex topLeft = createIndex(cell,0);
    QModelIndex bottomRight = createIndex(cell,1);
    qDebug() << "updateView topLeft: " << topLeft;
    emit dataChanged(topLeft, bottomRight);
}

Qt::ItemFlags MemoryModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QString MemoryModel::dissasm(struct MemoryWord MachineWord) const
{
    instruction inst;

    inst.opcode = MachineWord.data >> 57;
    inst.dest_mode = (MachineWord.data >> 54) & 0b0000000111;
    inst.src1_mode = (MachineWord.data >> 51) & 0b0000000000111;
    inst.src2_mode = (MachineWord.data >> 48) & 0b0000000000000111;
    inst.dest = (MachineWord.data << 16) >> 48;
    inst.src1 = (MachineWord.data << 32) >> 48;
    inst.src2 = (MachineWord.data << 48) >> 48;


    if (MachineWord.field >=128){
        QString assemblerText = opnames_matrix[inst.opcode].name + "\t";

        // destination operand
        if (inst.src1_mode == 6){   // offset
            assemblerText += "[" + QString::number(inst.src1) + "]";
            if (inst.dest_mode == 1) assemblerText += QString::number(inst.dest);
            if (inst.dest_mode == 2) assemblerText += "(" + QString::number(inst.dest) + ")";
        } else {
            switch (inst.dest_mode){
            default: break;
            case 0:     // opcode only instruction (sleep)
                return assemblerText;
            case 1:     // absolute
            case 4:     // src = dest
                assemblerText += QString::number(inst.dest);
                break;
            case 2:     // relative
                assemblerText += "(" + QString::number(inst.dest) + ")";
                break;
            case 3:     // system register
            case 5:     // src = dest
                assemblerText += sysregsnames_matrix[inst.dest].name;
            }
        }

        // source 1 operand
        if  (inst.src2_mode == 6){
            assemblerText += " ,[" + QString::number(inst.src2) + "]";
            if (inst.src1_mode == 1) assemblerText += QString::number(inst.src1);
            if (inst.src2_mode == 2) assemblerText += "(" + QString::number(inst.src1) + ")";
        } else {
            switch (inst.src1_mode){
            default: break;
            case 1:         // absolute
                assemblerText += " ," + QString::number(inst.src1);
                break;
            case 2:         // relative
                assemblerText += " ,(" + QString::number(inst.src1) + ")";
                break;
            case 3:         // immediate 16 bit
                assemblerText += ",#" + QString::number((qint16)inst.src1);
                break;
            case 4:         // immediate 32 bit
                return assemblerText += ",#" + QString::number((qint32)((inst.src1 << 16) | inst.src2));
            case 7:         // system register
                assemblerText += "," + sysregsnames_matrix[inst.src1].name;
                break;
            }
        }

        // source 2 operand
        switch(inst.src2_mode){
        default: break;
        case 1:             // absolute
            assemblerText += " ," + QString::number(inst.src2);
            break;
        case 2:         // relative
            assemblerText += " ,(" + QString::number(inst.src2) + ")";
            break;
        case 3:         // immediate 16 bit
            assemblerText += ",#" + QString::number((qint16)inst.src2);
            break;
        case 7:         // system register
            assemblerText += "," + sysregsnames_matrix[inst.src2].name;
            break;
        }

        return assemblerText;
    } else
        return "data";
}


/*MemoryWIndow */
quint64 MemoryWindow::getShardSize()
{
    return size;
}

quint64 MemoryWindow::getShardStart()
{
    return start;
}

quint64 MemoryWindow::getMemorySegmentNumber()
{
    return segment;
}

bool MemoryWindow::MemoryAccess (struct CpuMemoryInterface *interface)
{
    QMutexLocker locker (&memoryAccessMutex);
    qDebug() << "MemoryWindow::MemoryAccess Segment" << segment;
    if ((segment ^ (interface->address >> 40)) == 0) {              // not my segment, call router
        qDebug() << "in LocalMemoryAccess segment" << segment;
        return LocalMemoryAccess (interface);
    } else {
        qDebug() << "in RemoteMemoryAccess segment" << segment;
        return RemoteMemoryAccess(interface);
    }
}

/* called from RouterWindow Down?Mem->link((DataOnLink) info) */
bool MemoryWindow::LocalMemoryAccess (struct CpuMemoryInterface *interface)
{
    qDebug() << "MemoryWindow::LocalMemoryAccess Segment" << segment;
    if (interface->rw <= 1){                                                // do we have a memory access or signal
        if ((interface->address < start) || (interface->address > end)) {   // check if in range
            qDebug() << "LocalMemoryAccess out of range";
            return false;                                           // out of range, return false
        } else {
            qDebug() << "in local memory access";
            if (interface->rw == READ) {							// read
                qDebug() << "in Read";
                lock_rw.lockForRead();
                interface->word = shard [interface->address-start];
                lock_rw.unlock();
            } else {												// write
                qDebug() << "in write";
                lock_rw.lockForWrite();
                shard [interface->address-start] = interface->word;
                qDebug() << "interface->address: " << interface->address << " start: " << start << " address-start: " << interface->address-start;
//                memModel->updateMyView(interface->address-start, interface->word.data);
                memModel->updateView (interface->address-start);           // signal the TableView we have a change
                lock_rw.unlock();
            }
            return true;                                            // read or write successful
        }
    } else {                                                        // we have a signal
        qDebug() << "LocalMemoryAccess SIGNAL coming from Router";
        /* Pass Signal to Cpu if possible and return true. We don't return false as this is not a memory access */
        /* extract Cpu Number */
        quint8 cpuNumber = (interface->address & 0b1111);

        if (Cpu[cpuNumber])
            Cpu[cpuNumber]->receiveSignal(interface->word.data);
        qDebug() << "Signal sent to Cpu" << cpuNumber;

        return true;
    }
}

bool MemoryWindow::RemoteMemoryAccess (struct CpuMemoryInterface *interface)
{
    quint8 des_x, des_y, des_z;
    QString des_x_str, des_y_str, des_z_str;
    bool ok;
    quint8 xor_x, xor_y, xor_z, or_all;
    QString xor_x_str, xor_y_str, xor_z_str, or_all_str;

    quint8 up_info;
    QString up_info_str;
    quint32 down_info;
    QString down_info_str;

    qDebug() << "MeoryWindow::RemoteMemoryAccess Segment " <<  segment;
    QString dest_str = QString("%1").arg((interface->address >> 40),24,2,QChar('0'));
//    qDebug() << "destination string:" << dest_str;

//    qDebug() << "in Calculation";
    des_x_str = "";
    des_x_str.append(dest_str.at(2));
    des_x_str.append(dest_str.at(5));
    des_x_str.append(dest_str.at(8));
    des_x_str.append(dest_str.at(11));
    des_x_str.append(dest_str.at(14));
    des_x_str.append(dest_str.at(17));
    des_x_str.append(dest_str.at(20));
    des_x_str.append(dest_str.at(23));
    des_x = des_x_str.toUInt(&ok,2);
//    qDebug() << "des_x_str, des_x" << des_x_str << des_x;
    des_y_str = "";
    des_y_str.append(dest_str.at(1));
    des_y_str.append(dest_str.at(4));
    des_y_str.append(dest_str.at(7));
    des_y_str.append(dest_str.at(10));
    des_y_str.append(dest_str.at(13));
    des_y_str.append(dest_str.at(16));
    des_y_str.append(dest_str.at(19));
    des_y_str.append(dest_str.at(22));
    des_y = des_y_str.toUInt(&ok,2);
//    qDebug() << "des_y_str, des_y" << des_y_str << des_y;
    des_z_str = "";
    des_z_str.append(dest_str.at(0));
    des_z_str.append(dest_str.at(3));
    des_z_str.append(dest_str.at(6));
    des_z_str.append(dest_str.at(9));
    des_z_str.append(dest_str.at(12));
    des_z_str.append(dest_str.at(15));
    des_z_str.append(dest_str.at(18));
    des_z_str.append(dest_str.at(21));
    des_z = des_z_str.toUInt(&ok,2);
//    qDebug() << "des_z_st, des_z" << des_z_str << des_z;


    xor_x = des_x ^ src_x;
    xor_x_str = QString("%1").arg(xor_x,8,2,QChar('0'));
//    qDebug() << "xor_x" << xor_x;
    xor_y = des_y ^ src_y;
    xor_y_str = QString("%1").arg(xor_y,8,2,QChar('0'));
//    qDebug() << "xor_y" << xor_y;
    xor_z = des_z ^ src_z;
    xor_z_str = QString("%1"). arg(xor_z,8,2,QChar('0'));
//    qDebug() << "xor_z" << xor_z;

    or_all = xor_x | xor_y | xor_z;
    or_all_str = QString("%1").arg(or_all,8,2,QChar('0'));
    qDebug() << "or_all" << or_all;

    or_all >>= 1;
    qDebug() << "or_all nach >> 1" << or_all;

    up_info_str = "";
    down_info_str = "";

    down_info_str.append(des_z_str.at(7));
    down_info_str.append(des_y_str.at(7));
    down_info_str.append(des_x_str.at(7));
//    qDebug() << "bit 0, down info" << up_info_str << down_info_str;

    if (or_all >= 1) {   // 0
        up_info_str.append(QChar('1'));
        down_info_str.append(des_z_str.at(6));
        down_info_str.append(des_y_str.at(6));
        down_info_str.append(des_x_str.at(6));
//        qDebug() << "bit 1, down info" << up_info_str << down_info_str;
    }
    if (or_all >= 2) {   // 2
        up_info_str.append(QChar('1'));
        down_info_str.append(des_z_str.at(5));
        down_info_str.append(des_y_str.at(5));
        down_info_str.append(des_x_str.at(5));
//        qDebug() << "bit 2, down info" << up_info_str << down_info_str;
    }
    if (or_all >= 4) {   // 4
        up_info_str.append(QChar('1'));
        down_info_str.append(des_z_str.at(4));
        down_info_str.append(des_y_str.at(4));
        down_info_str.append(des_x_str.at(4));
//        qDebug() << "bit 3, down info" << up_info_str << down_info_str;
    }
    if (or_all >= 8) {   // 8
        up_info_str.append(QChar('1'));
        down_info_str.append(des_z_str.at(3));
        down_info_str.append(des_y_str.at(3));
        down_info_str.append(des_x_str.at(3));
//        qDebug() << "bit 4, down info" << up_info_str << down_info_str;
    }
    if (or_all >= 16) {  // 16
        up_info_str.append(QChar('1'));
        down_info_str.append(des_z_str.at(2));
        down_info_str.append(des_y_str.at(2));
        down_info_str.append(des_x_str.at(2));
//        qDebug() << "bit 5, down info" << up_info_str << down_info_str;
    }
    if (or_all >= 32) {  // 32
        up_info_str.append(QChar('1'));
        down_info_str.append(des_z_str.at(1));
        down_info_str.append(des_y_str.at(1));
        down_info_str.append(des_x_str.at(1));
//        qDebug() << "bit 6, down info" << up_info_str << down_info_str;
    }
    if (or_all >= 64) {  // 64
        up_info_str.append(QChar('1'));
        down_info_str.append(des_z_str.at(0));
        down_info_str.append(des_y_str.at(0));
        down_info_str.append(des_x_str.at(0));
//        qDebug() << "bit 7, down info" << up_info_str << down_info_str;
    }

qDebug() << "-+-+-+-+ up info, down info" << up_info_str << down_info_str;
    if (interface->rw == INJ){
        qDebug() << "MemoryWindow SIGNAL Injector";
        DataLink->UpInfo = 255;
    } else {
        DataLink->UpInfo = or_all;
    }
    DataLink->DownInfo = down_info_str.toUInt(&ok,2);
    DataLink->Mode = interface->rw;
    DataLink->Address = interface->address;
qDebug() << "interface address     data link address" << interface->address << DataLink->Address;
    DataLink->word = interface->word;

    // call router
    if (upLink->link(DataLink)) {
qDebug() << "router uplink success";
        interface->word = DataLink->word;
        return true;
    } else
        return false;
}

bool MemoryWindow::link (struct DataOnLink *DataLink)
{
    QMutexLocker locker (&linkMutex);
    struct CpuMemoryInterface interface;

    qDebug() << "Memory Window Link Segment" << segment;
    interface.address = DataLink->Address;
    interface.rw = DataLink->Mode;
    interface.word = DataLink->word;
    if (DataLink->Mode <= 1)
        if(LocalMemoryAccess(&interface)) {
            DataLink->word = interface.word;
            return true;
        } else
            return false;
    else {
        qDebug() << "LocalMemoryAccess SIGNAL coming from Router";
        /* Pass Signal to Cpu if possible and return true. We don't return false as this is not a memory access */
        /* extract Cpu Number */
        quint8 cpuNumber = (DataLink->Address & 0b0000000000000000000000000000000000000000000000000000000000001111);
        qDebug() << "DataLink->Address: " << DataLink->Address << " Cpu Number: " << cpuNumber;

        if (Cpu[cpuNumber])
            Cpu[cpuNumber]->receiveSignal(DataLink->word.data);
        qDebug() << "Signal sent to Cpu" << cpuNumber;

/*        switch (cpuNumber){
            case 0: if (Cpu0 != nullptr) Cpu0->receiveSignal(DataLink->word.data);
                    qDebug() << "Signal sent to Cpu0";
                    break;
            case 1: if (Cpu1 != nullptr) Cpu1->receiveSignal(DataLink->word.data);
                    qDebug() << "Signal sent to Cpu1";
                    break;
            case 2: if (Cpu2 != nullptr) Cpu2->receiveSignal(DataLink->word.data);
                    qDebug() << "Signal sent to Cpu2";
                    break;
            case 3: if (Cpu3 != nullptr) Cpu3->receiveSignal(DataLink->word.data);
                    qDebug() << "Signal sent to Cpu3";
                    break;
            default: qDebug() << "Signal sent to unknown Cpu";
        }
*/
        qDebug() << "Pass Signal to CPU -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
        return true;
    }
}

void MemoryWindow::on_displayTypeComboBox_activated(const QString &arg1)
{
    memModel->setDisplayType(ui->displayTypeComboBox->currentIndex());
    ui->mem_tableView->verticalHeader()->reset();
}
