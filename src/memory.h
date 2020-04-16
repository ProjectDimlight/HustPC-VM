#pragma once 

#include "definitions.h"
#include <string>
#include <exception>

// Allow un-aligned Read/Write
class Memory
{
public:

    virtual inline void OnClk()
    {
        // Virtualize EN and RW = 0
        if (read) OnRead();
        // Virtualize EN and RW = 1
        if (write) OnWrite();
        read = write = false;
        // Some devices need this signal for updating, such as PS/2 keyboard buffer
    }

    // External load byte
    virtual inline byte lb(uint index)
    {
        // Read signal
        read = true;
        return mem[index];
    }

    // External store byte
    virtual inline void sb(uint index, byte val)
    {
        // Write signal
        write = true;
        mem[index] = val;
    }

    virtual inline byte &operator[](uint id)
    {
        return mem[id];
    }

    inline void *GetArray()
    {
        return (void *)mem;
    }

    Memory(uint size = (1u<<20), std::string fileName = "");
    ~Memory();

protected:

    virtual inline void OnRead() { }
    virtual inline void OnWrite() { }

    // Internal operations

    virtual inline byte LB(uint index)
    {
        return mem[index];
    }

    virtual inline void SB(uint index, byte val)
    {
        mem[index] = val;
    }

    virtual inline half LH(uint index)
    {
        return LB(index) + (LB(index + 1) << 8);
    }

    virtual inline void SH(uint index, half val)
    {
        SB(index, val & 0xff);
        SB(index + 1, val >> 8);
    }

    virtual inline word LW(uint index)
    {
        return LH(index) + (LH(index + 2) << 16);
    }

    virtual inline void SW(uint index, word val)
    {
        SH(index, val & 0xffff);
        SH(index + 2, val >> 16);
    }

protected:

    byte *mem;
    bool read, write;
};