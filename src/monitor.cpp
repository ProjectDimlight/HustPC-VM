#include "monitor.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <cstdio>
#include <fstream>
#include <iostream>

using namespace std;

void Monitor::Flush()
{
    // Virtualize Monitor behaviour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Text mode
    //if (LW(size - 4) == TEXT)
    if (true)
    {
        half q, w;
        uint addr, rgba, tmp, t2, t3;
        for (uint i = 0, basec = 0, basep = 0; i < rowc; i++, basec += widthc, basep += colp * 16)
        for (uint j = 0; j < colc; j++)
        {
            tmp = LW(textStart + ((basec + j) << 2));
            q = (tmp >> 8) & 0xff;
            w = tmp & 0xff;
            addr = q == 0 ? w : (q-1) * 94 + (w-1) + 256;
            rgba = Color32b(tmp >> 16);
            
            for (uint r = 0; r < 16; r++)
            {
                for (uint c = 8; c < 16; c++)
                {
                    t2 = ((pixels[addr][r] >> (15-c)) & 1) ? rgba : 0;
                    t3 = (basep + r * colp + j * 16 + c - 8) << 2;
                    colors[t3 + 0] = t2;
                    colors[t3 + 1] = t2;
                    colors[t3 + 2] = t2;
                    colors[t3 + 3] = t2;
                }
                for (uint c = 0; c < 8; c++)
                {
                    t2 = ((pixels[addr][r] >> (15-c)) & 1) ? rgba : 0;
                    t3 = (basep + r * colp + j * 16 + c + 8) << 2;
                    colors[t3 + 0] = t2;
                    colors[t3 + 1] = t2;
                    colors[t3 + 2] = t2;
                    colors[t3 + 3] = t2;
                }
            }
        }
    }
    else
    {
        // Calculate Color for each Pixel
        for (uint i = 0, cnt = 0, base = 0, tmp; i < rowp; i++, base += widthp)
        {
            for (uint j = 0; j < colp; j++)
            {
                tmp = Color32b(LH((base + j) << 1));
                colors[cnt++] =  tmp;
                colors[cnt++] =  tmp;
                colors[cnt++] =  tmp;
                colors[cnt++] =  tmp;
            }
        }
    }
    
    // Bind Verticies
    glVertexPointer(3, GL_FLOAT, 0, verticies);

    // Bind Colors
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
        
    // Draw
    glDrawElements(GL_QUADS, 4 * rowp * colp, GL_UNSIGNED_INT, indicies);

    glutSwapBuffers();
}

uint Monitor::Min2Pow(uint t)
{
    uint i;
    for (i = 1; i && i < t; i <<= 1);

    if (i == 0)
        throw exception();
    return i;
}

void Monitor::InitGL()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
	glClearColor(0, 0, 0, 1);
	glClearDepth(1);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    gluOrtho2D(0, colp, 0, rowp);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
}

void Monitor::InitTextShape()
{
    FILE *fp = fopen("./hzk/mhzk16.zk", "r");
    for (uint i = 0, ignore; !feof(fp); i++)
    {
        ignore = fread(pixels[i], 2, 16, fp);
        if (i == 11548)
            printf("here\n");
    }
    fclose(fp);
    /*
    ifstream f("./hzk/hzk.coe");
    FILE *fp = fopen("./hzk/test", "w");
    char buf[256];
    f.getline(buf, 256);
    f.getline(buf, 256);
    int i, j, cnt;
    for (i = 0, j = 0, cnt = 0; cnt < 134960; i++)
    {
        for (j = 0; j < 16; j++, cnt++)
        {
            f.getline(buf, 256);
            sscanf(buf, "%hu", &pixels[i][j]);
            for (int t = pixels[i][j], p = 1 << 15, k = 0; k < 16; k++, t >>= 1)
                //fprintf(fp, t & p ? "**" : "  ");
                fprintf(fp, t & 1 ? "  " : "**");
            fprintf(fp, "\n");
        }
    }
    printf("OK\n");
    */
}

void Monitor::TestGraphics()
{
    for (int i = 0; i < rowp; i++)
    for (int j = 0; j < colp; j++)
    {
        SH((i * widthp + j) << 1, (15 << 0) + ((int)((float)i / rowp * 16) << 8) + ((int)((float)j / colp * 16) << 12));
    }
}

void Monitor::TestText()
{
    SW(size - 4, 1);
    uint tmp = rowp * widthp * 2;
    SW(tmp, 0xf0ff2443u); tmp += 4;
    SW(tmp, 0xff0f1a23u); tmp += 4;
    SW(tmp, 0xffff030cu); tmp += 4;
    SW(tmp, 0x0fff3736u); tmp += 4;
    SW(tmp, 0xffff0301u); tmp += 4;
    SW(tmp, 0xf0000000u); tmp += 4;
}

Monitor::Monitor(uint col, uint row) :
    //     Graphics Mode                                            Text Mode
    Memory(size = (textStart = row * (widthp = Min2Pow(col)) * 2) + (row / 16) * (widthc = Min2Pow(col / 16)) * 4 + 4)
{
    rowp = row, colp = col;
    rowc = row / 16; colc = col / 16;

    colors = new word[rowp * widthp * 4];
    //verticies = new float[(rowp + 1) * (colp + 1) * 3];
    verticies = new float[rowp * colp * 12];
    indicies = new uint[rowp * colp * 4];

    for (uint i = 0, cnt = 0; i < rowp; i++)
    for (uint j = 0; j < colp; j++)
    {
        // (j, i)
        verticies[cnt++] = j;
        verticies[cnt++] = rowp - i;
        verticies[cnt++] = 1;
        // (j+1, i)
        verticies[cnt++] = j + 1;
        verticies[cnt++] = rowp - i;
        verticies[cnt++] = 1;
        // (j+1, i+1)
        verticies[cnt++] = j + 1;
        verticies[cnt++] = rowp - i - 1;
        verticies[cnt++] = 1;
        // (j, i+1)
        verticies[cnt++] = j;
        verticies[cnt++] = rowp - i - 1;
        verticies[cnt++] = 1;
    }
    for (uint i = 0, cnt = 0; i < rowp; i++)
    for (uint j = 0; j < colp; j++)
    {
        indicies[cnt++] = cnt;
        indicies[cnt++] = cnt;
        indicies[cnt++] = cnt;
        indicies[cnt++] = cnt;
    }
    
    InitGL();
    InitTextShape();
}

Monitor::~Monitor()
{
    delete[] verticies;
    delete[] colors;
    delete[] indicies;
}