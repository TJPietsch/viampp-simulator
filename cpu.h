#ifndef CPU_H
#define CPU_H

#include <QObject>
#include <QThread>

#include "global.h"
//#include "cpuwindow.h"
//#include "memorywindow.h"

extern bool global_run;

union operands{
    quint64 u;
    qint64 s;
    qreal r;
};

class MemoryWindow;

class cpuThread : public QThread
{
    Q_OBJECT

public:
    explicit cpuThread(QObject *parent = nullptr);
    ~cpuThread();

    void setMemoryWindow(MemoryWindow *MemorySegment);
    void receiveSignal(quint64 data);
    void setSegmentCpu(quint64 segcpu);

signals:
    void updateProgramCounter(quint64 pc);
    void updateBaseRegisters(quint64 dest);
    void updateMinMax(quint64 min, quint64 max);
    void updateTimers(quint64 alice, quint64 bob, quint64 counter);
    void updateMessageWindow(QString message);
    void updateCPUOperation(struct instruction *b);
    void updateCPUOpcode(quint8 field, quint64 data);
    void updateResults(quint64 d, quint64 s1, quint64 s2);
    void updateTrapVector(quint16 index, quint64 value);
    void updateTrapStatus(quint16 d);
    void updateSignalValue(quint64 d);

protected:
    void run() override;

public slots:
    void controlSteps(quint8 steps, bool stepsActive);
    void controlStartStop (bool run);

private:
    MemoryWindow *MemorySegment;
    quint64 SegmentCpuNumber;

    struct CpuMemoryInterface *bus;

    // Viampp system registers
    quint64 SysRegs [26] = {0};

    quint64 CurrentSignal = 0;

    quint16 trapOccured = 0;
    quint8 steps = 1;
    bool active, steps_active = false;

    bool loadAbsolute(quint64 *result, quint16 operand, qint16 offset);
    bool loadRelative(quint64 *result, quint16 operand, qint16 offset);
    bool saveAbsolute(quint64 *result, quint16 operand, quint16 offset);
    bool saveRelative(quint64 *result, quint16 operand, quint16 offset);
};

#endif // CPU_H
