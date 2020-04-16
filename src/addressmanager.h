#pragma once

#include "memory.h"
#include <vector>

// Allow un-aligned Read/Write
class AddressManager
{
public:

    struct MemRange
    {
        uint from, to;
        Memory *item;
        bool read;
        bool write;
    };

    // 8-bit Byte Addressing
    // All based on memory modules' Load-Byte / Store-Byte Operations
    // Provide cross-module l/s operation

    // Mem1   Mem2 
    // ___[1||_3_]__

    inline byte LB(uint index)
    {
        auto tmp = locate(index);
        return tmp->item->lb(index - tmp->from);
    }

    inline void SB(uint index, byte val)
    {
        auto tmp = locate(index);
        return tmp->item->sb(index - tmp->from, val);
    }

    inline half LH(uint index)
    {
        return LB(index) + (LB(index + 1) << 8);
    }

    inline void SH(uint index, half val)
    {
        SB(index, val & 0xff);
        SB(index + 1, val >> 8);
    }

    inline word LW(uint index)
    {
        if (index == 0x08000014)
        {
            if (cnt == 0)
            {
                cnt = 1;
                return 65;
            } else if (cnt == 1)
            {
                cnt = 2;
                return 66;
            } else if (cnt == 2)
            {
                cnt = 3;
                return 67;
            } else {
                cnt = 0;
                return 0xff;
            }
        }
        return LH(index) + (LH(index + 2) << 16);
    }

    inline void SW(uint index, word val)
    {
        SH(index, val & 0xffff);
        SH(index + 2, val >> 16);
    }

    AddressManager(std::vector<MemRange> mems):
        mems(mems)
    {
        cnt = 0;
    }

    void OnClk()
    {
        for (auto &t : mems)
            t.item->OnClk();
    }

protected:
    std::vector<MemRange> mems;
    int cnt;

    // Find which address segment the given index belongs to
    inline MemRange *locate(uint index)
    {
        for (auto &t : mems)
        {
            // In range
            if (t.from <= index && index <= t.to)
                return &t;
        }
        return nullptr;
    }

};