#ifndef MEMORYWINDOW_H
#define MEMORYWINDOW_H

#include "global.h"

#include <QMainWindow>
#include <QAbstractTableModel>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <QDebug>
#include <QModelIndex>
#include <QVBoxLayout>

class RouterWindow;
//class cpuThread;
#include <cpu.h>

namespace Ui {
class MemoryWindow;
}

class MemoryModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    MemoryModel(quint64 start, quint64 size, MemoryWord *shard, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void updateView (quint64 cell);
    void updateMyView(quint64 cell, quint64 data);
    QString dissasm(struct MemoryWord MachinWord) const;
    void setDisplayType(quint8 type);

private:
    quint64 size;
    quint64 start;
    MemoryWord *shard;
    quint8 displayType = 0;

};

class MemoryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MemoryWindow(quint64 size, quint64 segment, RouterWindow *upLink, QWidget *parent = 0);
    virtual ~MemoryWindow();

    bool MemoryAccess (struct CpuMemoryInterface *interface);           // access memory or send to router
    bool LocalMemoryAccess (struct CpuMemoryInterface *interface);      // local memory access
    bool RemoteMemoryAccess (struct CpuMemoryInterface *interface);     // remote meory access
    bool link (struct DataOnLink *DataLink);                            // foreign memory access

    quint64 getShardSize ();
    quint64 getShardStart ();
    quint64 getMemorySegmentNumber();
    MemoryModel *memModel;

//    cpuThread *Cpu0, *Cpu1, *Cpu2, *Cpu3;
    cpuThread *Cpu[16];

private slots:
    void on_displayTypeComboBox_activated(const QString &arg1);

private:
    Ui::MemoryWindow *ui;

    quint64 size = 1024;        //default 1024 words
    quint64 start = 0;          // default 0
    quint64 end = 0;            // end = start + size
    quint64 segment = 0;        // segment number coming during creation

    MemoryWord *shard;
    QReadWriteLock lock_rw;
    RouterWindow *upLink;

    qint32 source;
    QString source_str;

    quint8 src_x, src_y, src_z;
    QString src_x_str, src_y_str, src_z_str;

    DataOnLink *DataLink;

    QMutex linkMutex, memoryAccessMutex;

};

#endif // MEMORYWINDOW_H
