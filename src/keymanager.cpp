#include "keymanager.h"

using namespace std;

vector<uint> KeyManager::charDownId[128] = 
{

};

vector<uint> KeyManager::charUpId[128] = 
{
    
};

vector<uint> KeyManager::specDownId[128] = 
{

};

vector<uint> KeyManager::specUpId[128] = 
{
    
};

// Set Key Code
void KeyManager::InitKeyCode()
{
    charDownId['0'] = {0x45};
    charDownId['1'] = {0x16};
    charDownId['2'] = {0x1E};
    charDownId['3'] = {0x26};
    charDownId['4'] = {0x25};
    charDownId['5'] = {0x2E};
    charDownId['6'] = {0x36};
    charDownId['7'] = {0x3D};
    charDownId['8'] = {0x3E};
    charDownId['9'] = {0x46};
    charDownId['9'] = {0x46};
    charDownId['x'] = charDownId['X'] = {0x22};
    charDownId['\r'] = {0x5A};
    charDownId['\b'] = {0x66};

    charUpId['0'] = {0xF0, 0x45};
    charUpId['1'] = {0xF0, 0x16};
    charUpId['2'] = {0xF0, 0x1E};
    charUpId['3'] = {0xF0, 0x26};
    charUpId['4'] = {0xF0, 0x25};
    charUpId['5'] = {0xF0, 0x2E};
    charUpId['6'] = {0xF0, 0x36};
    charUpId['7'] = {0xF0, 0x3D};
    charUpId['8'] = {0xF0, 0x3E};
    charUpId['9'] = {0xF0, 0x46};
    charUpId['x'] = charUpId['X'] = {0xF0, 0x22};
    charUpId['\r'] = {0xF0, 0x5A};
    charUpId['\b'] = {0xF0, 0x66};
}