#ifndef CPUWINDOW_H
#define CPUWINDOW_H

#include "global.h"
#include "cpu.h"
#include <QMainWindow>
#include <QDebug>

namespace Ui {
class CPUWindow;
}

class CPUWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CPUWindow(quint64 CpuNumber, MemoryWindow *MemorySegment, QWidget *parent = 0);
    ~CPUWindow();

    cpuThread *getCpuThreadPointer();

signals:
    void controlStartStop(bool running);
    void controlSteps(quint8 steps, bool stepsActive);

private slots:
    void on_CPUStartButton_clicked();
    void on_CPUStepButton_clicked();
    void on_CPUStopButton_clicked();
    void on_CPUStepspinBox_valueChanged(int arg1);

public slots:
    void onUpdateProgramCounter(quint64 pc);
    void updateBaseRegisters(quint64 dest);
    void updateMinMax(quint64 min, quint64 max);
    void updateTimers(quint64 alice, quint64 bob, quint64 counter);
    void updateMessageWindow (QString message);
    void updateCPUOperation(struct instruction *a);
    void updateCPUOpcode(quint8 field, quint64 data);
    void updateResults(quint64 d, quint64 s1, quint64 s2);
    void updateTrapVector(quint16 index, quint64 value);
    void updateTrapStatus(quint16 d);
    void updateSignalValue(quint64 d);

private:
    Ui::CPUWindow *ui;

    quint8 steps = 1;
    quint64 CpuNumber;
    MemoryWindow *MemorySegment;

    cpuThread cpu;

};

#endif // CPUWINDOW_H
