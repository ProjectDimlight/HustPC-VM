#include "../src/monitor.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>

using namespace std;

Monitor *m;

void test()
{
    m->Flush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, (char**) argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(640,480);
    glutCreateWindow("Hello OpenGL");
    glutDisplayFunc(test); 
    glutIdleFunc(test);
    //glutReshapeFunc(Reshape);

    m = new Monitor(640, 480);
    m->TestText();

    glutMainLoop();

    return 0;
}