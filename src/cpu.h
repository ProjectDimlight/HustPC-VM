#pragma once

class CPU
{
public:

    // Virtualize CPU behavior when clk signal pos-edge comes
    inline void OnClk()
    {
        Step();
        if (display)
            Display();
    }
    
    virtual void Debug() {}

    // Constructor
    CPU(bool displayInstructions):
        display(displayInstructions)
    {}

protected:

    virtual void Step() {}
    virtual void Display() {}

    bool display;
};