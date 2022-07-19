#include "cpuwindow.h"
#include "global.h"
#include "ui_cpuwindow.h"

#include "cpu.h"
#include "memorywindow.h"

CPUWindow::CPUWindow(quint64 CpuNumber, MemoryWindow *MemorySegment, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CPUWindow)
{
    ui->setupUi(this);

    this->CpuNumber = CpuNumber;
    this->MemorySegment = MemorySegment;

    setWindowTitle(QString("CPU Number %1 of Memory Segment %2").arg(CpuNumber).arg(MemorySegment->getMemorySegmentNumber()));
    qDebug () << "*CPU Window* CPU " << CpuNumber << " of Memory Segment " << MemorySegment->getMemorySegmentNumber() << "created";

    ui->CPUStepspinBox->setValue(1);

    ui->CPUProgramCounter->setText(QString::number(0));

    ui->CpuBaseAddressLabel->setText(QString::number(0));

    ui->CPUMinimumAddress->setText(QString::number(0));
    ui->CPUMaximumAddress->setText(QString::number(18446744073709551615U));

    ui->CPUTimerAlice->setText(QString::number(0));
    ui->CPUTimerBob->setText(QString::number(0));
    for (quint8 i = 0; i < 10; i++) updateTrapVector(i, 0);
    updateTrapStatus(0);

    ui->CPUStopButton->setEnabled(false);

    steps = 1;

    connect(this, SIGNAL(controlStartStop(bool)), &cpu, SLOT(controlStartStop(bool)));              // old style
    connect(this, SIGNAL(controlSteps(quint8,bool)), &cpu, SLOT(controlSteps(quint8,bool)));      // old style
    connect(&cpu, &cpuThread::updateProgramCounter, this, &CPUWindow::onUpdateProgramCounter);            // new style
    connect(&cpu, &cpuThread::updateTimers, this, &CPUWindow::updateTimers);
    connect(&cpu, &cpuThread::updateMessageWindow, this, &CPUWindow::updateMessageWindow);
    connect(&cpu, &cpuThread::updateCPUOperation, this, &CPUWindow::updateCPUOperation);
    connect(&cpu, &cpuThread::updateCPUOpcode, this, &CPUWindow::updateCPUOpcode);
    connect(&cpu, &cpuThread::updateResults, this, &CPUWindow::updateResults);
    connect(&cpu, &cpuThread::updateTrapVector, this, &CPUWindow::updateTrapVector);
    connect(&cpu, &cpuThread::updateTrapStatus, this, &CPUWindow::updateTrapStatus);
    connect(&cpu, &cpuThread::updateSignalValue, this, &CPUWindow::updateSignalValue);
    connect(&cpu, &cpuThread::updateBaseRegisters, this, &CPUWindow::updateBaseRegisters);


    cpu.setMemoryWindow(MemorySegment);
    cpu.setSegmentCpu((MemorySegment->getMemorySegmentNumber()<<40) + this->CpuNumber);
    cpu.start();
    ui->CPUOutput->insertPlainText("Ready\n\n");
}

CPUWindow::~CPUWindow()
{
    cpu.requestInterruption();
    cpu.wait();

    delete ui;
    qDebug () << "CPU Window " << CpuNumber << " of Memory Segment " << MemorySegment->getMemorySegmentNumber() << "destroyed!!!";
}

cpuThread *CPUWindow::getCpuThreadPointer()
{
    qDebug() << "+CPUWindow+getCpuThreadPointer: " << &cpu;
    return &cpu;
}

void CPUWindow::on_CPUStartButton_clicked()
{
    qDebug() << "Start Button clicked\n";
    ui->CPUStartButton->setEnabled(false);
    ui->CPUStepButton->setEnabled(false);
    ui->CPUStopButton->setEnabled(true);

    emit controlStartStop(true);
}

void CPUWindow::on_CPUStepButton_clicked()
{
    qDebug () << "CPU Step Button clicked\n";
    ui->CPUStartButton->setEnabled(false);
    ui->CPUStopButton->setEnabled(true);

    emit controlSteps(steps, true);
    emit controlStartStop(true);

}

void CPUWindow::on_CPUStopButton_clicked()
{
    qDebug() << "Stop Button clicked\n";

    ui->CPUStartButton->setEnabled(true);
    ui->CPUStepButton->setEnabled(true);
    ui->CPUStopButton->setEnabled(false);

    emit controlStartStop(false);
}

void CPUWindow::on_CPUStepspinBox_valueChanged(int arg1)
{
    steps = ui->CPUStepspinBox->value();
    qDebug() << "Steps set to: " << steps;
}

void CPUWindow::onUpdateProgramCounter(quint64 pc)
{
    ui->CPUProgramCounter->setText(QString::number(pc));
}

void CPUWindow::updateBaseRegisters(quint64 dest)
{
    ui->CpuBaseAddressLabel->setText(QString::number(dest));
}

void CPUWindow::updateMinMax(quint64 min, quint64 max)
{
    ui->CPUMinimumAddress->setText(QString::number(min));
    ui->CPUMaximumAddress->setText(QString::number(max));
}

void CPUWindow::updateTimers(quint64 alice, quint64 bob, quint64 counter)
{
    ui->CPUTimerAlice->setText(QString::number(alice));
    ui->CPUTimerBob->setText(QString::number(bob));
    ui->CPUCounter->setText(QString::number(counter));
}

void CPUWindow::updateMessageWindow(QString message)
{
    ui->CPUOutput->insertPlainText(message);
}

void CPUWindow::updateCPUOperation(struct instruction *inst)
{
    qDebug() << "updateCPUOperation";

    QString assemblerText = opnames_matrix[inst->opcode].name + "\t";

    // destination operand
    if (inst->src1_mode == 6){   // offset
        assemblerText += "[" + QString::number(inst->src1) + "]";
        if (inst->dest_mode == 1) assemblerText += QString::number(inst->dest);
        if (inst->dest_mode == 2) assemblerText += "(" + QString::number(inst->dest) + ")";
    } else {
        switch (inst->dest_mode){
        default: break;
        case 0:     // opcode only instruction (sleep)
            break;
        case 1:     // absolute
        case 4:     // src = dest
            assemblerText += QString::number(inst->dest);
            break;
        case 2:     // relative
            assemblerText += "(" + QString::number(inst->dest) + ")";
            break;
        case 3:     // system register
        case 5:     // src = dest
            assemblerText += sysregsnames_matrix[inst->dest].name;
        }
    }

    // source 1 operand
    if  (inst->src2_mode == 6){
        assemblerText += " ,[" + QString::number(inst->src2) + "]";
        if (inst->src1_mode == 1) assemblerText += QString::number(inst->src1);
        if (inst->src2_mode == 2) assemblerText += "(" + QString::number(inst->src1) + ")";
    } else {
        switch (inst->src1_mode){
        default: break;
        case 1:         // absolute
            assemblerText += " ," + QString::number(inst->src1);
            break;
        case 2:         // relative
            assemblerText += " ,(" + QString::number(inst->src1) + ")";
            break;
        case 3:         // immediate 16 bit
            assemblerText += ",#" + QString::number((qint16)inst->src1);
            break;
        case 4:         // immediate 32 bit
            assemblerText += ",#" + QString::number((qint32)((inst->src1 << 16) | inst->src2));
        case 7:         // system register
            assemblerText += "," + sysregsnames_matrix[inst->src1].name;
            break;
        }
    }

    // source 2 operand
    switch(inst->src2_mode){
    default: break;
    case 1:             // absolute
        assemblerText += " ," + QString::number(inst->src2);
        break;
    case 2:         // relative
        assemblerText += " ,(" + QString::number(inst->src2) + ")";
        break;
    case 3:         // immediate 16 bit
        assemblerText += ",#" + QString::number((qint16)inst->src2);
        break;
    case 7:         // system register
        assemblerText += "," + sysregsnames_matrix[inst->src2].name;
        break;
    }

    ui->CPUInstruction->setText(assemblerText);
    ui->CPUOpcode->setText(QString("%1 %2 %3 %4 %5 %6 %7").arg(inst->opcode,7,2,QChar('0'))
                                                          .arg(inst->dest_mode,3,2,QChar('0'))
                                                          .arg(inst->src1_mode,3,2,QChar('0'))
                                                          .arg(inst->src2_mode,3,2,QChar('0'))
                                                          .arg(inst->dest,16,2,QChar('0'))
                                                          .arg(inst->src1,16,2,QChar('0'))
                                                          .arg(inst->src2,16,2,QChar('0')));

/*
    ui->CPUDest->setText(QString("%1").arg(b->dest,16,2,QChar('0')));
    ui->CPUdestmode->setText(QString("%1").arg(b->dest_mode,3,2,QChar('0')));
    ui->CPUSource1->setText(QString("%1").arg(b->src1,16,2,QChar('0')));
    ui->CPUsrc1mode->setText(QString("%1").arg(b->src1_mode,3,2,QChar('0')));
    ui->CPUSource2->setText(QString("%1").arg(b->src2,16,2,QChar('0')));
    ui->CPUsrc2mode->setText(QString("%1").arg(b->src2_mode,3,2,QChar('0')));
*/
}

void CPUWindow::updateCPUOpcode(quint8 field, quint64 data)
{
//    ui->CPUOpcode->setText(QString("%1::%2").arg(field,8,2,QChar('0')).arg(data,64,2,QChar('0')));
}

void CPUWindow::updateResults(quint64 d, quint64 s1, quint64 s2)
{
    ui->CPUResult->setText(QString::number(d));
    ui->CPUOperand1->setText(QString::number(s1));
    ui->CPUOperand2->setText(QString::number(s2));
}

void CPUWindow::updateTrapVector(quint16 index, quint64 value)
{
    switch (index) {
        case 0: ui->TrapAddress0->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 1: ui->TrapAddress1->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 2: ui->TrapAddress2->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 3: ui->TrapAddress3->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 4: ui->TrapAddress4->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 5: ui->TrapAddress5->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 6: ui->TrapAddress6->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 7: ui->TrapAddress7->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 8: ui->TrapAddress8->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
        case 9: ui->TrapAddress9->setText(QString("0x%1").arg(value,16,16,QChar('0'))); break;
    }
}

void CPUWindow::updateTrapStatus(quint16 d)
{
    (d & 1) ? ui->TrapActive0->setText("active") : ui->TrapActive0->setText("-");
    (d & 2) ? ui->TrapActive1->setText("active") : ui->TrapActive1->setText("-");
    (d & 4) ? ui->TrapActive2->setText("active") : ui->TrapActive2->setText("-");
    (d & 8) ? ui->TrapActive2->setText("active") : ui->TrapActive2->setText("-");
    (d & 16) ? ui->TrapActive2->setText("active") : ui->TrapActive2->setText("-");
    (d & 128) ? ui->TrapActive2->setText("active") : ui->TrapActive2->setText("-");
    (d & 256) ? ui->TrapActive2->setText("active") : ui->TrapActive2->setText("-");
    (d & 512) ? ui->TrapActive2->setText("active") : ui->TrapActive2->setText("-");
}

void CPUWindow::updateSignalValue(quint64 d)
{
    ui->SignalValueLabel->setText(QString("0b%1").arg(d,64,2,QChar('0')));
}
