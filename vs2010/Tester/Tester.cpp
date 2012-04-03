// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PhySketchApplicationWindow.h"


int _tmain(int argc, _TCHAR* argv[])
{
	PhySketch::ApplicationWindow_WGL window;
	window.CreateApplicationWindow("test", 800, 600, false);
	return 0;
}

