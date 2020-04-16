objs = src/memory.o src/keymanager.o src/monitor.o src/mipscpu.o src/main.o
vgaobjs = src/memory.o src/monitor.o test/testVGA.o
CPPFLAGS = -std=c++17 -Wwrite-strings -O3
ogllibs = -lGL -lGLU -lglut
syslibs = -lpthread

build : $(objs)
	g++ -o bin/vm $(objs) $(ogllibs) $(syslibs)

testVGA : $(vgaobjs)
	g++ -o bin/testVGA $(vgaobjs) $(ogllibs)

clean :
	rm $(objs) $(vgaobjs)