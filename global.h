#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>
#include <QString>
#include <QMainWindow>
#include <QGraphicsItem>

enum rwm {
    READ = 0,		// read from memory
    WRITE,			// write to memory
    SIG,			// signal to cpu
    INJ             // signal to injector
};

const QString addrmode [] = {
    "immediate",
    "absolute",
    "indirect",
    "reserved",
    "predecrement",
    "postincrement",
    "precrement",
    "postcrement"
};

enum addressing_mode {
    IMMEDIATE = 0,
    ABSOLUTE,
    INDIRECT,
    RESERVED,
    PREDECREMENT,
    POSTINCREMENT,
    PRECREMENT,
    POSTCREMENT
};

struct instruction {
    bool bit71;
    quint8 type;
    quint8 opcode;
    quint8 dest_mode;
    quint8 src1_mode;
    quint8 src2_mode;
    quint16 dest;
    quint16 src1;
    quint16 src2;
}; Q_DECLARE_METATYPE(instruction)

struct MemoryWord {
    quint8 field;
    quint64 data;
}; Q_DECLARE_METATYPE(MemoryWord)

struct DataOnLink {
    quint16 UpInfo;
    quint32 DownInfo;
    quint8 Mode;
    quint64 Address;
    struct MemoryWord word;
};

struct CpuMemoryInterface {
    quint64 address;
    struct MemoryWord word;
    int rw;
};

struct mpp_element_router {
    quint8 subType;     // down link connected to router or memory
    quint8 subTypeUp;   // up link connected to router or injector
    quint8 routerNumber;
    quint8 routerLevel;
    struct mpp_element *Down[8];
};

struct mpp_element_memory {
    quint64 size;
    quint64 segment;
    struct mpp_element *Cpu[16];
};

struct mpp_element_cpu
{
    quint64 cpu_number;
};

struct mpp_element {
    quint8 mpp_type;
    QMainWindow *mpp_element_window_ptr;
    QGraphicsItem *mpp_element_icon_ptr;
    quint16 x;
    quint16 y;
    QString description;
    struct mpp_element *Up;
    union {
        struct mpp_element_router mpp_router;
        struct mpp_element_memory mpp_memory;
        struct mpp_element_cpu mpp_cpu;
    };
};

enum sysregs {
    PC,
    BaseAddr,
    MinAddr,
    MaxAddr,
    SigVal,
    Alice,
    Bob,
    Counter,
    InOut,
    TrapMask,
    PrevPc,
    Config,

    SigVec = 15,
    InpVec,
    AliceVec,
    BobVec,
    RangeVec,
    BaddrVec,
    IllOpVec,
    CBVec,
    OvflVec,
    DivVec
};

const struct sysregsnames {
    QString name;
    QString description;
} sysregsnames_matrix[] = {
{"PC", "Program Counter"},
{"BaseAddr", "Base Address"},
{"MinAddr", "Minimum Address"},
{"MaxAddr", "Maximum Address"},
{"SigVal", "Signal Value"},
{"Alice", "Counter Alice"},
{"Bob", "Counter Bob"},
{"Counter", "Free Running Counter"},
{"InOut", "Input / Output Port"},
{"TrapMask", "Active / Inactive Traps"},
{"PrevPc", "Previous Program Counter"},
{"Config", "Configuration (Tile/CPU)"},
{"reserved","reserved"},
{"reserved","reserved"},
{"reserved","reserved"},
{"reserved","reserved"},
{"SigVec", "Signal Trap Vector"},
{"InpVec", "Input Trap Vector"},
{"AliceVec", "Alice Trap Vector"},
{"BobVec", "Bob Vector"},
{"RangeVec", "Address out of Range Trap Vector"},
{"BaddrVec", "Bad Address Trap Vector"},
{"IllOpVec", "Illegal Opcode Trap Vector"},
{"CBVec", "Carry/Borrow Trap Vector"},
{"OvflVec", "Overflow on Multiplication Trap Vector"},
{"DivVec", "Division by Zero Trap Vector"},
};

enum opcodes {
    ADD = 1,
    ADDU,
    SUB,
    SUBU,
    MUL,
    MULU,
    DIV,
    DIVU,
    REMS,
    REMU,

    MIN,
    MINU,
    MAX,
    MAXU,

    MOV,
    MOVH,

    ABS,
    BITREV,
    BSWAP,
    CTLZ,
    CTTZ,
    CTBS,

    AND,
    OR,
    XOR,
    NAND,
    NOR,
    NXOR,
    NOT,

    SB,
    CB,

    LSL,
    LSR,
    ASR,
    RL,
    RR,

    BEQ,
    BNE,
    BG,
    BGU,
    BGE,
    BGEU,
    BL,
    BLU,
    BLE,
    BLEU,
    BBS,
    BBC,

    SLEEP,
    SIGNAL,

    FADD,
    FSUB,
    FMUL,
    FDIV,
    FREM,
    FABS,
    FMIN,
    FMAX,
    FNEG,

    FP2SI,
    FP2UI,
    SI2FP,
    UI2FP,

    FRINT,
    FTRUNC,
    FCEIL,
    FFLOOR,

    FPOW,
    FSQRT,
    FSIN,
    FCOS,
    FEXP2,
    FLOG2,
    FEXP,
    FLOG,

    FBEQ,
    FBNE,
    FBG,
    FBGE,
    FBL,
    FBLE

};

const struct opnames {
        QString name;
        QString description;
        QChar dest;
        QChar src1;
        QChar src2;
        bool update_pc;
        bool show_dest;
        bool show_src1;
        bool show_src2;
} opnames_matrix [] = {
{"add", "Addition", 'w', 'r', 'r', true, true, true, true},
{"addu", "Unsigned Addition", 'w', 'r', 'r', true, true, true, true},
{"sub", "Subtraction", 'w', 'r', 'r', true, true, true, true},
{"subu", "Unsigned Subtraction", 'w', 'r', 'r', true, true, true, true},
{"mul", "Multiplication", 'w', 'r', 'r', true, true, true, true},
{"mulu", "Unsigned Multiplication", 'w', 'r', 'r', true, true, true, true},
{"div", "Division", 'w', 'r', 'r', true, true, true, true},
{"divu", "Unsigned Division", 'w', 'r', 'r', true, true, true, true},
{"rems","Reminder", 'w', 'r', 'r', true, true, true, true},
{"remu", "Unsigned Reminder", 'w', 'r', 'r', true, true, true, true},
{"min", "Minimum", 'w', 'r', 'r', true, true, true, true},
{"minu", "Unsigned Minimum", 'w', 'r', 'r', true, true, true, true},
{"max", "Maximum", 'w', 'r', 'r', true, true, true, true},
{"maxu", "Unsigned Maximum", 'w', 'r', 'r', true, true, true, true},
{"mov", "Move", 'w', ' ', 'r', true, true, false, true},
{"mov.h", "Move to Upper", 'w', ' ', 'r', true, true, false, true},
{"abs", "Absolute", 'w', ' ', 'r', true, true, false, true},
{"bitrev", "Bitreverse", 'w', ' ', 'r', true, true, false, true},
{"bswap", "Swap Halfwords", 'w', ' ', 'r', true, true, false, true},
{"ctlz", "Count leading Zeros", 'w', ' ', 'r', true, true, false, true},
{"cttz", "Count trailing Zeros", 'w', ' ', 'r', true, true, false, true},
{"ctbs", "Count bits set", 'w', ' ', 'r', true, true, false, true},
{"and", "Logic AND", 'w', 'r', 'r', true, true, true, true},
{"or", "Logic OR", 'w', 'r', 'r', true, true, true, true},
{"xor", "Logic XOR", 'w', 'r', 'r', true, true, true, true},
{"nand", "Logic NAND", 'w', 'r', 'r', true, true, true, true},
{"nor", "Logic NOR", 'w', 'r', 'r', true, true, true, true},
{"nxor", "Logic Not XOR", 'w', 'r', 'r', true, true, true, true},
{"not", "Logic NOT", 'w', 'r', ' ', true, true, true, false},
{"sb", "Set Bit", 'w', 'r', 'r', true, true, true, true},
{"cb", "Clear Bit", 'w', 'r', 'r', true, true, true, true},
{"lsl", "Logical Shift Left", 'w', 'r', 'r', true, true, true, true},
{"lsr", "Logical Shift Right", 'w', 'r', 'r', true, true, true, true},
{"asr", "Arithmetic Shift Right", 'w', 'r', 'r', true, true, true, true},
{"rl", "Rotate Left", 'w', 'r', 'r', true, true, true, true},
{"rr", "Rotate Right", 'w', 'r', 'r', true, true, true, true},
{"beq", "Branch on Equal", 'r', 'r', 'r', false, true, true, true},
{"bne", "Branch on Not Equal", 'r', 'r', 'r', false, true, true, true},
{"bg", "Branch on Greater", 'r', 'r', 'r', false, true, true, true},
{"bgu", "Unsigned Branch on Greater", 'r', 'r', 'r', false, true, true, true},
{"bge", "Branch on Greater or Equal", 'r', 'r', 'r', false, true, true, true},
{"bgeu", "Unsigned Branch on Greater or Equal", 'r', 'r', 'r', false, true, true, true},
{"bl", "Branch on Less", 'r', 'r', 'r', false, true, true, true},
{"blu", "Unsigned Branch on Less", 'r', 'r', 'r', false, true, true, true},
{"ble", "Branch on Less or Equal", 'r', 'r', 'r', false, true, true, true},
{"bleu", "Unsigned Branch on Less or Equal", 'r', 'r', 'r', false, true, true, true},
{"bbs", "Branch on Bit Set", 'r', 'r', 'r', false, true, true, true},
{"bbc", "Branch on Nit Clear", 'r', 'r', 'r', false, true, true, true},
{"sleep", "Goto Sleep mode", ' ', ' ', ' ', true, false, false, false},

{"fadd", "FP Addition", 'w', 'r', 'r', true, true, true, true},
{"fsub", "FP Subtraction", 'w', 'r', 'r', true, true, true, true},
{"fmul", "FP Multiplication", 'w', 'r', 'r', true, true, true, true},
{"fdiv", "FP Division", 'w', 'r', 'r', true, true, true, true},
{"fabs", "FP Absolute", 'w', ' ', 'r', true, true, false, true},
{"fmin", "FP Minimum", 'w', 'r', 'r', true, true, true, true},
{"fmax", "FP Maximum", 'w', 'r', 'r', true, true, true, true},
{"fneg", "FP Negation", 'w', ' ', 'r', true, true, false, true},
{"fp2si", "FP to signed Integer", 'w', ' ', 'r', true, true, false, true},
{"fp2ui", "FP to unsigned Integer", 'w', ' ', 'r', true, true, false, true},
{"si2fp", "Signed Integer to FP", 'w', ' ', 'r', true, true, false, true},
{"ui2fp", "Unsigned Integer to FP", 'w', ' ', 'r', true, true, false, true},
{"frint", "Round to nearest Integer", 'w', ' ', 'r', true, true, false, true},
{"ftrunc", "Truncate to Integer", 'w', ' ', 'r', true, true, false, true},
{"fceil", "Ceiling function", 'w', ' ', 'r', true, true, false, true},
{"ffloor", "Floor function", 'w', ' ', 'r', true, true, false, true},
{"fpow", "Power", 'w', ' ', 'r', true, true, false, true},
{"fsqrt", "Square Root", 'w', ' ', 'r', true, true, false, true},
{"fsin", "Sinus", 'w', ' ', 'r', true, true, false, true},
{"fcos", "Cosinus", 'w', ' ', 'r', true, true, false, true},
{"fexp2", "Exponent (Base 2)", 'w', ' ', 'r', true, true, false, true},
{"flog2", "Logarithm (Base 2)", 'w', ' ', 'r', true, true, false, true},
{"fexp", "Exponent (Base e)", 'w', ' ', 'r', true, true, false, true},
{"flog", "Logarithm (Base e)", 'w', ' ', 'r', true, true, false, true},
{"fbeq", "Branch on Equal", 'r', 'r', 'r', false, true, true, true},
{"fbne", "Branch on Not Equal", 'r', 'r', 'r', false, true, true, true},
{"fbg", "Branch on Greater", 'r', 'r', 'r', false, true, true, true},
{"fbge", "Branch on Greater or Equal", 'r', 'r', 'r', false, true, true, true},
{"fbl", "Branch on Less", 'r', 'r', 'r', false, true, true, true},
{"fble", "Branch on Less or Equal", 'r', 'r', 'r', false, true, true, true},
};


#endif // GLOBAL_H
