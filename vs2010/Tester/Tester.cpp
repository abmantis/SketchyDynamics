// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PhySketchCore.h"


int _tmain(int argc, _TCHAR* argv[])
{
	PhySketch::Core core;
	core.createWindow("test", 800, 600, false);
	core.startLoop();
	return 0;
}

