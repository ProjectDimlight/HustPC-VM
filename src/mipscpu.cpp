#include "mipscpu.h"
#include <sstream>
#include <cstring>

using namespace std;

enum OpCode
{
    R = 0x00,
    BEQ = 0x04,
    BNE = 0x05,
    JAL = 0x03,
    LW = 0x23,
    SW = 0x2b,
    ADDI = 0x08,
    ORI = 0x0d,
    XORI = 0x0e,
    LUI = 0x0f,
    STOP = 0x3f,
};

enum FuncCode
{
    AND = 0x24,
    OR = 0x25,
    ADD = 0x20,
    SUB = 0x22,
    SRL = 0x02,
    SRA = 0x03,
    SLL = 0x00,
    JR = 0x08,
    XOR = 0x26,
    SLT = 0x2A
};

// Name of registers
// Used when displaying instructions
const string MipsCPU::name[32] = {
    "$zero",
    "$at",
    "$v0",
    "$v1",
    "$a0",
    "$a1",
    "$a2",
    "$a3",
    "$t0",
    "$t1",
    "$t2",
    "$t3",
    "$t4",
    "$t5",
    "$t6",
    "$t7",
    "$s0",
    "$s1",
    "$s2",
    "$s3",
    "$s4",
    "$s5",
    "$s6",
    "$s7",
    "$t8",
    "$t9",
    "$k0",
    "$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra"
};

void MipsCPU::Debug()
{
    printf("%10u    0x%08x   %s\n", opc / 4 + 1, ir, c2s(ir).c_str());
    for (int i = 0; i < 32; i++)
    {
        printf("%6s: %9d ", name[i].c_str(), regs[i]);
        if (i % 4 == 3)
            printf("\n");
    }
    printf("\n");
}

// Legacy code from Assembler
string MipsCPU::c2s(uint s)
{
    // 系统调用
    if (s == 0)
        return "nop";
    else if(s == 0x0000000c)
        return "syscall";

    ostringstream res;
    word val = getVal(s, 31, 26);

    // I类型
    // 存取
    if(val == LW)  // lw
    {
        res << "lw " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0) << "(" << name[getVal(s, 25, 21)] << ")";
    }
    else if(val == SW)  // sw
    {
        res << "sw " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0) << "(" << name[getVal(s, 25, 21)] << ")";
    }
    else if(val == LUI) // lui
    {
        res << "lui " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0);
    }
    // 跳跃
    else if(val == BEQ)  // beq
    {
        res << "beq " << name[getVal(s, 25, 21)] << ", " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0);
    }
    else if(val == BNE)  // beq
    {
        res << "bne " << name[getVal(s, 25, 21)] << ", " << name[getVal(s, 20, 16)] << ", " << getVal(s, 15, 0);
    }
    // 运算
    else if(val == ORI) // ori
    {
        res << "ori " << name[getVal(s, 20, 16)] << ", " << name[getVal(s, 25, 21)] << ", " << getVal(s, 15, 0);
    }
    else if(val == ADDI) // addi
    {
        res << "addi " << name[getVal(s, 20, 16)] << ", " << name[getVal(s, 25, 21)] << ", " << getVal(s, 15, 0);
    }
    // R类型
    else if(val == R )
    {
        word type = getVal(s, 5, 0);
        if(type == ADD)
            res << "add";
        else if(type == SUB)
            res << "sub";
        else if(type == SLL)
            res << "sll";
        // Not implemented... yet
        else if(type == SLT)
            res << "slt";
        else if(type == JR)
        {
            res << "jr " << name[getVal(s, 25, 21)];
            return res.str();
        } 

        res << " " << name[getVal(s, 15, 11)] << ", " << name[getVal(s, 25, 21)] << ", " << name[getVal(s, 20, 16)];
    }
    // J类型
    else if(val == JAL)
    {
        res << "jal " << getVal(s, 25, 0);
    }
    
    return res.str();
}

void MipsCPU::Step()
{
    if (!stop)
    {
        // Delay slot
        if (dlc == delay)
        {
            // jump or branch to target
            pc = target;
            target = -1;
            dlc = 0;
        }else if (target != -1)
        {
            // delay count
            dlc++;
        }

        // Record original PC for display
        opc = pc;

        IF();
        IR();
        E2W();
        if (ir == 0x3C13FFFF)
        {
            printf("%d: %d %d %d\n", pc, regs[1], regs[16], regs[18]);
        }
    }
}

void MipsCPU::IF()
{
    // Load Instruction
    ir = am->LW(pc);
    pc = pc + 4;
}

void MipsCPU::IR()
{
    // Load parts from IR register
    op = ir >> 26;
    func = ir & 0x3f;

    rs = (ir >> 21) & 0x1f;
    rt = (ir >> 16) & 0x1f;
    rd = (ir >> 11) & 0x1f;

    shamt = (ir >> 6) & 0x1f;

    immu = ir & 0xffff;
    imm = (int)((immu & 0x8000) ? (immu + 0xffff0000) : immu);
}

// E2W = Exe + Mem + Write Back
void MipsCPU::E2W()
{
    switch (op)
    {
    case R:
        switch (func)
        {
        case AND:
            regs[rd] = regs[rs] & regs[rt];
            break;
        case OR:
            regs[rd] = regs[rs] | regs[rt];
            break;
        case ADD:
            regs[rd] = regs[rs] + regs[rt];
            break;
        case SUB:
            regs[rd] = regs[rs] - regs[rt];
            break;
        case SRL:
            regs[rd] = regs[rt] >> shamt;
            break;
        case SRA:
            regs[rd] = ((int)regs[rt]) >> shamt;
            break;
        case SLL:
            regs[rd] = regs[rt] << shamt;
            break;
        case XOR:
            regs[rd] = regs[rs] ^ regs[rt];
            break;
        case SLT:
            regs[rd] = regs[rs] < regs[rt];
            break;
        case JR:
            pc = regs[31];
            break;
        }
        break;
    case BNE:
        if (regs[rs] != regs[rt])
            target = pc + (imm << 2);
        break;
    case BEQ:
        if (regs[rs] == regs[rt])
            target = pc + (imm << 2);
        break;
    case JAL:
        regs[31] = pc;
        target = (pc & 0xf0000000) | ((ir & 0x03ffffff) << 2);
        break;
    case LW:
        regs[rt] = am->LW(imm + regs[rs]);
        break;
    case SW:
        am->SW(imm + regs[rs], regs[rt]);
        break;
    case ADDI:
        regs[rt] = regs[rs] + imm;
        break;
    case ORI:
        regs[rt] = regs[rs] | immu;
        break;
    case XORI:
        regs[rt] = regs[rs] ^ immu;
        break;
    case LUI:
        regs[rt] = immu << 16;
        break;
    case STOP:
        stop = true;
        break;
    default:
        // NOP
        break;
    }
}

MipsCPU::MipsCPU(AddressManager *am, bool display, uint initPC, uint delaySlots):
    CPU(display)
{
    pc = initPC;
    delay = delaySlots;
    dlc = 0;
    target = -1;
    ir = 0;
    stop = 0;

    memset(regs, 0, sizeof(regs));

    this->am = am;
}

