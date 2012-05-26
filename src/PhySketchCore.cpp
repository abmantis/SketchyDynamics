#include "PhySketchCore.h"

#ifdef PHYSKETCH_OS_WIN32
	#include "PhySketchApplicationWindow_WGL.h"
#endif


namespace PhySketch
{
template<> Core* Singleton<Core>::ms_Singleton = 0;

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

Core* Core::getSingletonPtr( void )
{
	assert(ms_Singleton != NULL);
	return ms_Singleton;
}

Core& Core::getSingleton( void )
{
	assert(ms_Singleton != NULL);
	return *ms_Singleton;
}

}
