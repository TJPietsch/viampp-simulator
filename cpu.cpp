#include "cpu.h"

#include "memorywindow.h"
#include <QtMath>

cpuThread::cpuThread(QObject *parent /*= nullptr*/) : QThread(parent)
{
    bus = new CpuMemoryInterface;

    SysRegs[MaxAddr] =  18446744073709551615U;
}

cpuThread::~cpuThread()
{
    delete bus;
}

void cpuThread::run()
{
    operands d, s1, s2;
    struct instruction inst;

    while(!isInterruptionRequested()){
        while (global_run && active){
            if (steps_active) {
                if(steps > 1){
                    steps--;
                } else {
                    steps_active = false;
                    active = false;
                }
            }

     /* ---------- Timer Ticks & Check ---------- */
            if (SysRegs[TrapMask] & 4){     // is Timer Alice active
                SysRegs[Alice]--;           // decrement Alice
                if (SysRegs[Alice] <= 0) {  // Trap occured
                    trapOccured |= 4;       // set trap indicator
                    goto next_cycle;        // execute trap in next cycle
                }
            }

            if (SysRegs[TrapMask] & 8){     // is Timer Bob active
                SysRegs[Bob]--;             // decrement Bob
                if (SysRegs[Bob] <= 0) {    // Trap occured
                    trapOccured |= 8;       // set trap indicator
                    goto next_cycle;        // execute trap in next cycle
                }
            }

            SysRegs[Counter]++;

            emit updateTimers(SysRegs[Alice], SysRegs[Bob], SysRegs[Counter]);  // update Timers in cpuwindow
            emit updateProgramCounter(SysRegs[PC]);     // update Program Counter in cpuwindow


    /* ---------- Trap occured ---------- */
            if (trapOccured != 0){          // check if trap occured
                qDebug() << "Trap occured: " << trapOccured;
                if (trapOccured & 0b1100000000000000) {     // all non maskable: Address out of range, Bad Address, Illegal Opcode
                    qDebug() << " not maskable trap";
                    if (trapOccured & 32) SysRegs[PC] = SysRegs[BaddrVec];  // Bad Address trap
                    if (trapOccured & 64) SysRegs[PC] = SysRegs[IllOpVec];  // Illegal opcode trap
                    trapOccured &= 0b0011111111111111;                      // clear non maskable trap flags
                    goto next_cycle;                                        // execute trap in next cycle
                } else
                    if (trapOccured & 0b0000000000000001) {                 // Signal detected
                        if (SysRegs[TrapMask & 0b0000000000000001]) {       // is trap on signal enabled
                            qDebug() << " Signal trap";
                            SysRegs[PC] = SysRegs[SigVec];                  // set pc to trap vector
                            trapOccured &= 0b1111111111111110;              // clear trap flag
                            goto next_cycle;                                // execute trap in next cycle
                        }
                    } else
                        if (trapOccured & 0b0000000000000010) {             // Input detected
                            if (SysRegs[TrapMask & 0b0000000000000010]) {   // is trap on input enabled
                                qDebug() << " Signal trap";
                                SysRegs[PC] = SysRegs[InpVec];              // set pc to trap vector
                                trapOccured &= 0b1111111111111101;          // clear trap flag0
                                goto next_cycle;                            // execute trap in next cycle
                            }
                        } else
                            if (trapOccured & 0b0000000000000100) {         // Timer Alice detected
                                if (SysRegs[TrapMask & 0b0000000000000100]) {
                                    qDebug() << "input port trap";
                                    SysRegs[PC] = SysRegs[AliceVec];
                                    trapOccured &= 0b1111111111111011;
                                    goto next_cycle;
                                }
                            } else
                                if (trapOccured & 0b0000000000001000) {     // Timer Bob detected
                                    if (SysRegs[TrapMask & 0b0000000000001000]) {
                                        qDebug() << "Alice overflow trap";
                                        SysRegs[PC] = SysRegs[BobVec];
                                        trapOccured &= 0b1111111111110111;  // clear trap
                                        goto next_cycle;
                                    }
                                } else
                                    if  (trapOccured & 0b0000000000010000) {     // address out of range
                                        if (SysRegs[TrapMask & 0b0000000000010000]) {
                                            qDebug() << "Carry/Borrow overflow trap";
                                            SysRegs[PC] = SysRegs[CBVec];
                                            trapOccured &= 0b1111111111101111;  // clear trap
                                            goto next_cycle;
                                        }
                                    } else
                                        if  (trapOccured & 0b0000000000100000) {     // carry/borrow detected
                                            if (SysRegs[TrapMask & 0b0000000000100000]) {
                                                qDebug() << "Carry/Borrow overflow trap";
                                                SysRegs[PC] = SysRegs[CBVec];
                                                trapOccured &= 0b1111111110111111;  // clear trap
                                                goto next_cycle;
                                            }
                                         } else
                                            if (trapOccured & 0b0000000001000000) { // overflow on multiplication detected
                                                if (SysRegs[TrapMask & 0b0000000001000000]) {
                                                    qDebug() << "Multiplication overflow trap";
                                                    SysRegs[PC] = SysRegs[CBVec];
                                                    trapOccured &= 0b1111111110111111;  // clear trap
                                                    goto next_cycle;
                                                }
                                            } else
                                                if (trapOccured & 0b0000000010000000) { // division by zero detected
                                                    if (SysRegs[TrapMask & 0b0000000010000000]) {
                                                        qDebug() << "Division by zero trap";
                                                        SysRegs[PC] = SysRegs[CBVec];
                                                        trapOccured &= 0b1111111101111111;  // clear trap
                                                        goto next_cycle;
                                                    }
                                                }

            }       // end if (trapOccured != 0)


    /* ---------- Fetch Instruction ----------- */
            qDebug() << "Fetch Instruction\n";
            bus->address = SysRegs[PC];
            bus->rw = READ;
            if (!MemorySegment->MemoryAccess(bus)) {
                qDebug() << "BAD Address\n";
                emit updateMessageWindow("--- BAD Address (Fetch Instruction) !!!\n");
                trapOccured |= 0b010000000000000;      // Bad Address Trap
                goto next_cycle;
            }
            emit updateCPUOpcode(bus->word.field, bus->word.data);

    /* ---------- Decode Instruction ---------- */
            qDebug() << "Decode Instruction Operands\n";
            if ((bus->word.field) < 128) {
                qDebug() << "Illegal Instruction\n";
                emit updateMessageWindow("--- Illegal Instruction !!!\n");
                trapOccured |= 0b1000000000000000;      // Illegal Opcode Trap
                goto next_cycle;
            }

            inst.opcode = bus->word.data >> 57;
            inst.dest_mode = (bus->word.data >> 54) & 0b0000000111;
            inst.src1_mode = (bus->word.data >> 51) & 0b0000000000111;
            inst.src2_mode = (bus->word.data >> 48) & 0b0000000000000111;
            inst.dest = (bus->word.data << 16) >> 48;
            inst.src1 = (bus->word.data << 32) >> 48;
            inst.src2 = (bus->word.data << 48) >> 48;

    /* ---------- Fetch Operands ---------- */
            qDebug() << "Fetch Operands\n";
            switch (inst.src2_mode){    // src2 operand
            default: break;
            case 1: // absolute
                if (!loadAbsolute(&s2.u, inst.src2, 0)) goto next_cycle;
                break;
            case 2: // relative
                if (!loadRelative(&s2.u, inst.src2, 0)) goto next_cycle;
                break;
            case 3: // immediate 16 bit
                s2.s = (qint16)inst.src2;
                break;
            case 4: // immediate 32 bit
                s2.s = (qint32)((inst.src1 << 16) | inst.src2);
                break;
            case 6: // offset
                if (inst.src1_mode == 1){    // absolute + offset
                    if (!loadAbsolute(&s2.u, inst.src1, inst.src2)) goto next_cycle;
                    break;
                }
                if (inst.src1_mode == 2){    // relative + offset
                    if (!loadRelative(&s2.u, inst.src1, inst.src2)) goto next_cycle;
                    break;
                }
            case 7: // system register
                s2.u = SysRegs[inst.src2 % 16];
                break;
            }

            if (inst.src2 != 6)             // if this is not an offset, get the s1 operand
                switch (inst.src1_mode){    // src1 operand
                default: break;
                case 1: // absolute
                    if (!loadAbsolute(&s1.u, inst.src1, 0)) goto next_cycle;
                    break;
                case 2: // relative
                    if (!loadRelative(&s1.u, inst.src1, 0)) goto next_cycle;
                    break;
                case 3: // immediate 16 bit
                    s1.s = (qint16)inst.src1;
                    break;
                case 7: // system register
                    s1.u = SysRegs[inst.src1 % 16];
                    break;
                }

            switch (inst.dest_mode){   // source = dest
            default: break;
            case 4: // absolute
                if (!loadAbsolute(&s1.u, inst.dest, 0)) goto next_cycle;
                break;
            case 2: // relative
                if (!loadRelative(&s1.u, inst.dest, 0)) goto next_cycle;
                break;
            }


    /* ---------- Execute Instruction ---------- */
            qDebug() << "Execute Instruction\n";
            switch(inst.opcode){                                // execute opcode
            case ADD:   // signed addition
                if (SysRegs[TrapMask] & 0b0000000000100000){    // is over/underflow trap active, then check for it
                    if (((s2.s > 0) && (s1.s > LLONG_MAX - s2.s)) || ((s2.s < 0) && (s1.s < LLONG_MIN - s2.s))){
                        trapOccured |= 0b0000000000100000;
                        goto next_cycle;
                    }
                }
                d.s = s1.s + s2.s;
                break;
            case ADDU:  // unsigned addition
                if (SysRegs[TrapMask] & 0b0000000000100000){    // is over/underflow trap active, then check for it
                    if ((s1.u > ULLONG_MAX - s2.u)){
                        trapOccured |= 0b0000000000100000;
                        goto next_cycle;
                    }
                }
                d.u = s1.u + s2.u;
                break;
            case SUB:   // signed subtraction
                if (SysRegs[TrapMask] & 0b0000000000100000){    // is over/underflow trap active, then check for it
                    if (((s2.s < 0) && (s1.s > LLONG_MAX + s2.s)) || ((s2.s > 0) && (s1.s < LLONG_MIN + s2.s))){
                        trapOccured |= 0b0000000000100000;
                        goto next_cycle;
                    }
                }
                d.u = s1.u - s2.u;
                break;
            case SUBU:  // unsigned subtraction
                if (SysRegs[TrapMask] & 0b0000000000100000){    // is over/underflow trap active, then check for it
                    if ((s1.u > ULLONG_MAX + s2.u)){
                        trapOccured |= 0b0000000000100000;
                        goto next_cycle;
                    }
                }
                d.u = s1.u - s2.u;
                break;
            case MUL:   // signed multiplication
                if (SysRegs[TrapMask] & 0b0000000001000000){    // is over/underflow trap active, then check for it
                    if ((s1.s > LLONG_MAX / s2.s) || (s1.s < LLONG_MIN / s2.s)){
                        trapOccured |= 0b0000000001000000;
                        goto next_cycle;
                    }
                }
                d.s = s1.s * s2.s;
                break;
            case MULU:  // unsigned multiplication
                if (SysRegs[TrapMask] & 0b0000000001000000){    // is over/underflow trap active, then check for it
                    if (s1.u > ULLONG_MAX / s2.u){
                        trapOccured |= 0b0000000001000000;
                        goto next_cycle;
                    }
                }
                d.u = s1.u * s2.u;
                break;
            case DIV:   // signed division
                if (SysRegs[TrapMask] & 0b0000000010000000){    // is over/underflow trap active, then check for it
                    if (s2.s == 0){
                        trapOccured |= 0b0000000010000000;
                        goto next_cycle;
                    }
                }
                d.s = s1.s / s2.s;
                break;
            case DIVU:  // unsigned division
                if (SysRegs[TrapMask] & 0b0000000010000000){    // is over/underflow trap active, then check for it
                    if (s2.u == 0){
                        trapOccured |= 0b0000000010000000;
                        goto next_cycle;
                    }
                }
                d.u = s1.u / s2.u;
                break;
            case REMS:  // signed reminder
                if (SysRegs[TrapMask] & 0b0000000010000000){    // is over/underflow trap active, then check for it
                    if (s2.s == 0){
                        trapOccured |= 0b0000000010000000;
                        goto next_cycle;
                    }
                }
                d.s = s1.s % s2.s;
                break;
            case REMU:  // unsigned reminder
                if (SysRegs[TrapMask] & 0b0000000010000000){    // is over/underflow trap active, then check for it
                    if (s2.u == 0){
                        trapOccured |= 0b0000000010000000;
                        goto next_cycle;
                    }
                }
                d.u = s1.u % s2.u;
                break;
            case MIN:  d.s = (s1.s < s2.s) ? s1.s : s2.s;
                break;
            case MINU: d.u = (s1.u < s2.u) ? s1.u : s2.u;
                break;
            case MAX:  d.s = (s1.s > s2.s) ? s1.s : s2.s;
                break;
            case MAXU: d.u = (s1.u > s2.u) ? s1.u : s2.u;
                break;
            case MOV: d = s2;
                break;
            case MOVH: d.u |= ((s2.u << 32) & 0xffffffffffffffff);
                break;
            case ABS: d.s = abs(s2.s);
                break;
            case BITREV:
            case BSWAP:
            case CTLZ:
            case CTTZ:
            case CTBS:
                break;
            case AND:  d.u = s1.u & s2.u;
                break;
            case OR:   d.u = s1.u | s2.u;
                break;
            case XOR:  d.u = s1.u ^ s2.u;
                break;
            case NAND: d.u = ~ (s1.u & s2.u);
                break;
            case NOR:  d.u = ~ (s1.u | s2.u);
                break;
            case NXOR: d.u = ~ (s1.u | s2.u);
                break;
            case NOT:  d.u = ~s2.u;
                break;
            case SB:   d.u = s1.u | (1 << s2.u);
                break;
            case CB:   d.u = s1.u & (~ (1 << s2.u));
                break;
            case LSL: d.u = s1.u << s2.u;
                break;
            case LSR:  d.u = s1.u >> s2.u;
                break;
            case ASR:
                if (s1.s < 0)
                    d.s = (s1.s >> s2.s) | ~(~0U >> s2.s);
                else
                    d.s = s1.s >> s2.s;
                break;
            case RL:   //s2 &= 63;
                d.u = (s1.u << s2.u) | (s1.u >> (64 - s2.u));
                break;
            case RR:   //s2 &= 63;
                d.u = (s1.u >> s2.u) | (s1.u << (64 - s2.u));
                break;
            case BEQ:  if (d.u == s1.u) SysRegs[PC] += s2.s;
                break;
            case BNE:  if (d.u != s1.u) SysRegs[PC] += s2.s;
                break;
            case BG:   if (d.s > s1.s) SysRegs[PC] += s2.s;
                break;
            case BGU:  if (d.u > s1.u) SysRegs[PC] += s2.s;
                break;
            case BGE:  if (d.s >= s1.s) SysRegs[PC] += s2.s;
                break;
            case BGEU: if (d.u >= s1.u) SysRegs[PC] += s2.s;
                break;
            case BL:   if (d.s < s1.s) SysRegs[PC] += s2.s;
                break;
            case BLU:  if (d.u < s1.u) SysRegs[PC] += s2.s;
                break;
            case BLE:  if (d.s <= s1.s) SysRegs[PC] += s2.s;
                break;
            case BLEU: if (d.u <= s1.u) SysRegs[PC] += s2.s;
                break;
            case BBS:  if (d.u & (1 << s1.u)) SysRegs[PC] += s2.s;
                break;
            case BBC:  if (!(d.u & (1 << s1.u))) SysRegs[PC] += s2.s;
                break;
            case SLEEP: steps = 0;
                steps_active = false;
                active = false;
                emit updateMessageWindow("Going to SLEEP!\n");
                break;

            case FADD: d.r = s1.r + s2.r;
                break;
            case FSUB: d.r = s1.r - s2.r;
                break;
            case FMUL: d.r = s1.r * s2.r;
                break;
            case FDIV: d.r = s1.r / s2.r;
                break;
            case FABS: d.r = qAbs(s2.r);
                break;
            case FMIN: d.r = (s1.r < s2.r) ? s1.r : s2.r;
                break;
            case FMAX: d.r = (s1.r < s2.r) ? s1.r : s2.r;
                break;
            case FP2SI: d.s = (qint64)s2.r;
                break;
            case FP2UI: d.u = (quint64)s2.r;
                break;
            case SI2FP: d.r = (qreal)s2.s;
                break;
            case UI2FP: d.r = (qreal)s2.u;
                break;
            case FRINT: // ----------------------------------------------------------
            case FTRUNC: // ---------------------------------------------------------
                break;
            case FCEIL: d.s = qCeil(s2.r);
                break;
            case FFLOOR: d.s = qFloor(s2.r);
                break;
            case FPOW: d.r = qPow(s1.r, s2.r);
                break;
            case FSQRT: d.r = qSqrt(s2.r);
                break;
            case FSIN: d.r = qSin(s2.r);
                break;
            case FCOS: d.r = qCos(s2.r);
                break;
            case FEXP2: // ----------------------------------------------------------
            case FLOG2: d.r = qLn(s2.r) / qLn(2);
                break;
            case FEXP: d.r = qExp(s2.r);
                break;
            case FLOG: d.r = qLn(s2.r);
                break;
            case FBEQ: if (d.r == s1.r) SysRegs[PC] += s2.s;
                break;
            case FBNE: if (d.r != s1.r) SysRegs[PC] += s2.s;
                break;
            case FBG: if (d.r > s1.r) SysRegs[PC] += s2.s;
                break;
            case FBGE: if (d.r >= s1.r) SysRegs[PC] += s2.s;
                break;
            case FBL: if (d.r < s1.r) SysRegs[PC] += s2.s;
                break;
            case FBLE: if (d.r <= s1.r) SysRegs[PC] += s2.s;
                break;

            default:   qDebug() << "Illegal Instruction\n";
                emit updateMessageWindow("--- Illegal Instruction !!!\n"); // trap (5);
                trapOccured |= 0b1000000000000000;     // Illegal OpcodeTrap
                goto next_cycle;
            }   // end switch

    /* ---------- Write Back Result ---------- */
            if (inst.src1_mode == 6){       // write back with offset
                if (inst.dest_mode == 1)    // absolute with offset
                    if (!saveAbsolute(&d.u, inst.dest, inst.src1)) goto next_cycle;
                if (inst.dest_mode == 2)    // relative with offeset
                    if (!saveRelative(&d.u, inst.dest, inst.src1)) goto next_cycle;
            } else {
                switch(inst.dest_mode){
                default: break;
                case 1:                     // absolute
                case 4:                     // absolute src=dest
                    if (!saveAbsolute(&d.u, inst.dest, 0)) goto next_cycle;
                    break;
                case 2:                     // write back relative
                    if (!saveRelative(&d.u, inst.dest, 0)) goto next_cycle;
                    break;
                case 3:                     // system register
                case 5: {                   // system register src=dest
                    quint32 systemReg = inst.dest % 16;
                    SysRegs[systemReg] = d.u;
                    if (systemReg == TrapMask){
                        SysRegs[TrapMask] &= 0x00000000000001cf;
                        emit updateTrapStatus(SysRegs[TrapMask]);
                    }
                    if (systemReg >= 16 && systemReg <= 25) emit updateTrapVector(systemReg, SysRegs[systemReg]);
                    break;
                }
                }
            }

    /* ---------- Update CPU operations and results ---------- */
            if (!active){                               // no update if steps greater 1 or run mode
                emit updateCPUOperation(&inst);
                emit updateResults(d.u,s1.u,s2.u);
            }

    /* ---------- Update Program Counter ---------- */
            if (opnames_matrix[inst.opcode].update_pc)
                SysRegs[PC]++;

next_cycle:;

        } // end while loop
    } // end while interrupted loop
}

bool cpuThread::loadAbsolute (quint64 *result, quint16 operand, qint16 offset) {
    bus->address = operand + SysRegs[BaseAddr] + offset;        // setup memory interface
    if ((SysRegs[TrapMask] & 0b0000000000010000) && (bus->address < SysRegs[MinAddr] || bus->address > SysRegs[MaxAddr])) {     // do address range check
        qDebug() << "Address out of range\n";
        emit updateMessageWindow("--- Address out of range !!!\n");
        trapOccured |= 0b0000000000010000;          // setup address range trap
        return false;                               // trap occured
    }
    bus->rw = READ;
    if (MemorySegment->MemoryAccess(bus)) {         // access memory
        *result = bus->word.data;                   // prepare operand
        return true;
    } else {
        qDebug() << "BAD Address\n";                // bad address trap
        emit updateMessageWindow("--- BAD Address !!!\n");
        trapOccured |= 0b0100000000000000;          // setup bad address trap
        return false;                               // trap occured
    }
    qDebug() << "absolute operand " << *result;

    return true;
}

bool cpuThread::loadRelative(quint64 *result, quint16 operand, qint16 offset) {
    bus->address = operand + SysRegs[BaseAddr];        // setup memory interface
    if ((SysRegs[TrapMask] & 0b0000000000010000) && (bus->address < SysRegs[MinAddr] || bus->address > SysRegs[MaxAddr])) {      // do address range check
        qDebug() << "Address out of range\n";
        emit updateMessageWindow("--- Address out of range !!!\n");
        trapOccured |= 0b0000000000010000;          // setup address range trap
        return false;                               // trap occured
    }
    bus->rw = READ;
    if (MemorySegment->MemoryAccess(bus)) {         // access memory
        bus->address = bus->word.data + offset;              // setup memory interface
        if ((SysRegs[TrapMask] & 0b0000000000010000) && (bus->address < SysRegs[MinAddr] || bus->address > SysRegs[MaxAddr])) {  // do address range check
            qDebug() << "Address out of range\n";
            emit updateMessageWindow("--- Address out of range !!!\n");
            trapOccured |= 0b0000000000010000;      // setup address range trap
            return 2;                               // trap occured
        }
        bus->rw = READ;
        if (MemorySegment->MemoryAccess(bus)) {     // access memory
            *result = bus->word.data;               // prepare operand
            return true;
        } else {
            qDebug() << "BAD Address\n";            // bad address trap
            emit updateMessageWindow("--- BAD Address !!!\n");
            trapOccured |= 0b0100000000000000;      // setup bad address trap
            return false;                           // trap occured
        }
    } else {
            qDebug() << "BAD Address\n";            // bad address trap
            emit updateMessageWindow("--- BAD Address !!!\n");
            trapOccured |= 0b0100000000000000;      // setup bad address trap
            return false;                           // trap occured
    }
    qDebug() << "indirect operand " << *result;

    return true;
}

bool cpuThread::saveAbsolute(quint64 *result, quint16 operand, quint16 offset) {
    bus->word.field = 0;
    bus->address = operand + SysRegs[BaseAddr] + offset;
    if ((SysRegs[TrapMask] & 0b0000000000010000) && (bus->address < SysRegs[MinAddr] || bus->address > SysRegs[MaxAddr])) {     // do address range check
        qDebug() << "Address out of range\n";
        emit updateMessageWindow("--- Address out of range !!!\n");
        trapOccured |= 0b0000000000010000;          // setup address range trap
        return false;                               // trap occured
    }
    bus->word.data = *result;
    bus->rw = WRITE;
    if (!MemorySegment->MemoryAccess(bus)) {
        qDebug() << "BAD Address\n";
        emit updateMessageWindow("--- BAD Address !!!\n");
        trapOccured |= 0b0100000000000000;          // setup bad address trap
        return false;                               // trap occured
    }

    return true;
}


bool cpuThread::saveRelative(quint64 *result, quint16 operand, quint16 offset){
    bus->word.field = 0;
    bus->address = operand + SysRegs[BaseAddr];
    if ((SysRegs[TrapMask] & 0b0000000000010000) && (bus->address < SysRegs[MinAddr] || bus->address > SysRegs[MaxAddr])) {     // do address range check
        qDebug() << "Address out of range\n";
        emit updateMessageWindow("--- Address out of range !!!\n");
        trapOccured |= 0b0000000000010000;          // setup address range trap
        return false;                               // trap occured
    }
    bus->rw = READ;
    if (MemorySegment->MemoryAccess(bus)) {
        bus->address = bus->word.data + offset;
        if ((SysRegs[TrapMask] & 0b0000000000010000) && (bus->address < SysRegs[MinAddr] || bus->address > SysRegs[MaxAddr])) { // do address range check
            qDebug() << "Address out of range\n";
            emit updateMessageWindow("--- Address out of range !!!\n");
            trapOccured |= 0b0000000000010000;      // setup address range trap
            return false;                           // trap occured
        }
        bus->word.data = *result;
        bus->rw = WRITE;
        if (!MemorySegment->MemoryAccess(bus)) {
            qDebug() << "BAD Address\n";
            emit updateMessageWindow("--- BAD Address !!!\n");
            trapOccured |= 0b0100000000000000;      // setup bad address trap
            return false;                           // trap occured
        }
    } else {
        qDebug() << "BAD Address\n";
        emit updateMessageWindow("--- BAD Address !!!\n");
        trapOccured |= 0b0100000000000000;          // setup bad address trap
        return false;                               // trap occured
    }

    return true;
}

void cpuThread::controlSteps(quint8 stepsToDo, bool stepsActive)
{
    steps = stepsToDo;
    steps_active = stepsActive;
    qDebug() << "controlSteps " << steps << steps_active << stepsToDo;
}

void cpuThread::controlStartStop(bool run)
{
    active = run;
    qDebug() << "controlStartStop " << active;
}

void cpuThread::setMemoryWindow(MemoryWindow *MemorySegment)
{
//    qDebug() << "cpuThread MemorySegment " << MemorySegment;
    this->MemorySegment = MemorySegment;
}

void cpuThread::setSegmentCpu(quint64 segcpu)
{
    SegmentCpuNumber = segcpu;
}

void cpuThread::receiveSignal(quint64 data)
{
    qDebug() << "Signal in cpuThread received";
    CurrentSignal = data;
    trapOccured |= 1;       // Signal Detected Trap
    emit updateSignalValue(data);
    emit updateMessageWindow("Signal received\n");
    active = true;
}
