#include "instruction.h"

Instruction::Instruction()
{

}

instruction Instruction::Decode(quint64 data){
    instruction inst;

    inst.opcode = data >> 57;
    inst.dest_mode = (data >> 54) & 0b0000000111;
    inst.src1_mode = (data >> 51) & 0b0000000000111;
    inst.src2_mode = (data >> 48) & 0b0000000000000111;
    inst.dest = (data << 16) >> 48;
    inst.src1 = (data << 32) >> 48;
    inst.src2 = (data << 48) >> 48;

    return inst;
}

QString Instruction::Dissassemble(MemoryWord MachineWord){
    instruction inst;

    if (MachineWord.field >=128){
        inst = Decode(MachineWord.data);

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
