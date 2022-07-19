#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "global.h"

class Instruction
{
public:
    Instruction();

    instruction Decode(quint64 data);
    QString Dissassemble(MemoryWord mem);
};

#endif // INSTRUCTION_H
