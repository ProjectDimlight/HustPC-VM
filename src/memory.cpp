#include "memory.h"
#include <exception>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

using namespace std;

Memory::Memory(uint size, string f)
{
    mem = new byte[size];
    memset(mem, 0, sizeof(byte) * size);
    read = write = false;

    if (f != "")  // Load coe-like binary code
    {
        FILE *fp = fopen(f.c_str(), "r");
        char s[20] = "0x";
        for (uint i = 0; i < size && fscanf(fp, "%s", s + 2) != EOF; i+=4)
        {
            sscanf(s, "%x", (int*)(mem + i));
        }
        fclose(fp);
    }
}

Memory::~Memory()
{
    delete[] mem;
}