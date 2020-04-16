#pragma once
#include "definitions.h"
#include "memory.h"
#include <exception>
#include <queue>
#include <vector>

class KeyManager : public Memory
{
public:
    
    KeyManager(uint row, uint col);
    ~KeyManager();
    
    inline void Down(unsigned char chr)
    {
        for (auto &x : charDownId[chr])
            q.push(x);
    }

    inline void Down(int spec)
    {
        for (auto &x : specDownId[spec])
            q.push(x);
    }

    inline void Up(unsigned char chr)
    {
        for (auto &x : charUpId[chr])
            q.push(x);
    }

    inline void Up(int spec)
    {
        for (auto &x : specUpId[spec])
            q.push(x);
    }

    inline byte lb(uint index) override
    {
        read = true;
        if (q.empty())
            return 0;
        else
        {
            uint t = q.front();
            return t >> (index * 8);
        }
    }

    inline void sb(uint index, byte val) override
    {
        // do nothing
    }

    void debug()
    {
        tmp = q;
        printf("Queue: ");
        while (!tmp.empty())
        {
            printf("%02x ", tmp.front());
            tmp.pop();
        }
        printf("\n");
    }

    static std::vector<uint> charDownId[128];
    static std::vector<uint> specDownId[128];
    static std::vector<uint> charUpId[128];
    static std::vector<uint> specUpId[128];

    // Initialize Key Code
    // Must be called when initialize
    static void InitKeyCode();

    KeyManager():
        Memory(1)
    {

    }

protected:

    inline void OnRead() override
    {
        if (!q.empty())
            q.pop();
    }

    std::queue<uint> q, tmp;

    bool charDown[128];
    bool specDown[128];
};