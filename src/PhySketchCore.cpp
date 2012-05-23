#include "PhySketchCore.h"
#include "PhySketchApplicationWindow.h"

namespace PhySketch
{

Core::Core( void )
{
	_window = nullptr;
}

Core::~Core( void )
{
	if(_window != nullptr)
	{
		delete _window;
	}
}

ApplicationWindow* Core::createWindow( std::string title, int width, int height, bool fullscreen )
{
	if(_window != nullptr)
	{
		delete _window;
	}

#ifdef PHYSKETCH_OS_WIN32
	_window = new ApplicationWindow_WGL();
#endif
	

	_window->createWindow( title, width, height, fullscreen);

	return _window;
}

void Core::startLoop()
{
	bool done = false;

	while(!done)
	{
		if(!_window->updateWindow())
		{
			std::cout << "Window closing" << std::endl;
			done = true;
		}	
	}

	
}

}
