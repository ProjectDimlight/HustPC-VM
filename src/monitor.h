#pragma once
#include "definitions.h"
#include "memory.h"
#include <exception>

#define GRAPHICS 0
#define TEXT 1

class Monitor : public Memory
{
public:

    void Flush();
    void SetMode(bool mode);
    Monitor(uint row, uint col);
    ~Monitor();
    
    void TestGraphics();
    void TestText();

protected:

    inline word Color32b(half val)
    {
        byte tmp[4];
        for (int i = 0, j; i < 4; i++)
        {
            j = 3-i;
            tmp[j] = val & 0xf;
            tmp[j] = (tmp[j] << 4) + (tmp[j]);
            val >>= 4;
        }
        return *(word *)tmp;
    }

    inline uint locate(uint zb)
    {
        if ((zb & 0xE000) == 0)        // ASCii、其它符号
            return zb - 0x2000;        // 字库中位置
        else if ((zb & 0x8000) == 0)   // 汉字字符
            return (zb - 0x2000 + 256);
        return 0;
    }

    inline uint Min2Pow(uint t);
    void InitGL();
    void InitTextShape();

    float *verticies;
    uint *indicies;
    word *colors;

    uint rowp, colp;  // Size in Pixels
    uint rowc, colc;  // Size in charactors
    uint widthp, widthc;

    uint size, textStart;
    
    half pixels[100000][16];
};