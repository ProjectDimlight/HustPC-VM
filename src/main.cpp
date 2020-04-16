#include "memory.h"
#include "mipscpu.h"
#include "monitor.h"
#include "keymanager.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>

#include <cstring>
#include <thread>
#include <iostream>

using namespace std;

//////////////////////////// VM ////////////////////////////

class VirtualMachine
{
public:

    void Init(char *fn, bool display)
    {
        // VRAM
        monitor = new Monitor(640, 480);
        // Inst Mem
        mem0 = new Memory(2048 * 4, fn);
        // Data Mem
        mem1 = new Memory(8192 * 4, "./asm/keycode.coe");
        // Input 
        keyBuf = new KeyManager();
        KeyManager::InitKeyCode();

        mem2 = new Memory(2048 * 4, fn);

        // Address Manager
        am = new AddressManager({
            {0, 2048 * 4 - 1, mem0},
            {2048 * 4, 10240 * 4 - 1, mem1},
            {10240 * 4, 257921 * 4 - 1, monitor},
            {0x08000000, 0x08000003, keyBuf},
            {0x08000010, 0x0800001F, mem2}
        });

        debug = display;
        cpu = new MipsCPU(am, false);
    }

    void KeyDown(unsigned char c)
    {
        keyBuf->Down(c);
    }

    void KeyUp(unsigned char c)
    {
        keyBuf->Up(c);
    }

    void OnClk()
    {
        cpu->OnClk();
        am->OnClk();

        if (debug)
        {
            monitor->Flush();
            Debug();
            getchar();
        }
        else
        {
            if (cnt == 10000)
            {
                monitor->Flush();
                cnt = 0;
            }else
                cnt++;
        }
    }

    void Debug()
    {
        cpu->Debug();
    }

private:

    int cnt = 0;
    Monitor *monitor;
    Memory *mem0, *mem1, *mem2;
    KeyManager *keyBuf;
    AddressManager *am;
    CPU *cpu;
    bool debug;
} vm;

// Virtualize key-down
void keyDown(unsigned char c, int x, int y)
{
    vm.KeyDown(c);
}

// Virtualize key-up
void keyUp(unsigned char c, int x, int y)
{
    vm.KeyUp(c);
}

void VM()
{
    vm.OnClk();
}

//////////////////////////// CLI ////////////////////////////

class CLI
{
public:

    CLI(VirtualMachine *vm):
        vm(vm)
    {
    }

    void Exec(string s)
    {
        if (s == "debug")
        {

        }
        else if (s == "display")
        {
            vm->Debug();
        }
        else
        {
            cout << "Invalid command." << endl;
        }
    }

protected:
    
    VirtualMachine *vm;
} cli(&vm);

// CLI event main loop
void CLI()
{
    for (string s; ; )
    {
        cout << ">>> ";
        getline(cin, s);
        cli.Exec(s);
    }
}

//////////////////////////// Main ////////////////////////////

int main(int argc, char *argv[])
{
    glutInit(&argc, (char**) argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(640,480);
    glutCreateWindow("Virtual Monitor");
    glutDisplayFunc(VM);
    glutIdleFunc(VM);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutIgnoreKeyRepeat(1);
    
    bool debug = (argc == 3 && strcmp(argv[2], "-d") == 0);

    vm.Init(argv[1], debug);

    // fork CLI thread
    if (!debug)
    {
        thread cli(CLI);
        cli.detach();
    }

    // Virtual Machine main thread
    glutMainLoop();

    return 0;
}