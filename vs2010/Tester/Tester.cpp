// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "PhySketchCore.h"
#include "PhySketchApplicationWindow.h"

#include "TestInputListener.h"

PhySketch::Core _core;
PhySketch::ApplicationWindow *_window;
TestInputListener *_inputListener;

int _tmain(int argc, _TCHAR* argv[])
{	
	new PhySketch::Logger("Log.txt", true);
	_window = _core.createWindow("test", 800, 600, false);
	
	_inputListener = new TestInputListener();
	_window->addInputListener(_inputListener);
	
	_core.startLoop();

	delete _inputListener; _inputListener = 0;
	return 0;
}

