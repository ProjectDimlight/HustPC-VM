#pragma once

#include "cpu.h"
#include "addressmanager.h"
#include "definitions.h"
#include <string>

class MipsCPU : public CPU
{
public:

    void Debug() override;

    MipsCPU(AddressManager *am, bool displayInstryctions, uint initPC = 0, uint delaySlots = 1);

protected:

    // Old Translator
    std::string c2s(uint s);

    inline word getVal(word a, int r, int l)
    {
        return (a & (((r == 31 ? 0 : 1u << (r + 1))) - 1)) >> l; 
    }

    const static std::string name[32];

    // Override Functions

    void Step() override;
    inline void Display() override
    {
        printf("%10u    0x%08x   %s\n", opc, ir, c2s(ir).c_str());
    }

    // Mips Stages

    void IF();
    void IR();
    void E2W();  // E2W = Exe + Mem + WB
    
    AddressManager *am;
    word regs[32];

    // opc := original pc, used for Display()
    uint opc, pc, ir, target;
    uint delay, dlc;

    uint op, func, immu;
    int imm;
    uint rs, rt, rd, shamt;

    bool stop;
};