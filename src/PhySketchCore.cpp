#include "PhySketchCore.h"

#ifdef PHYSKETCH_OS_WIN32
	#include "PhySketchApplicationWindow_WGL.h"
#endif
#include "PhySketchLogger.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsManager.h"


namespace PhySketch
{
template<> Core* Singleton<Core>::ms_Singleton = 0;

Core::Core( void )
{
	_window = nullptr;
	_logger = nullptr;
	_mainInputListener = nullptr;
	_renderer = nullptr;
	_physicsMgr = nullptr;
}

Core::~Core( void )
{
	if(_window != nullptr)
	{
		delete _window; _window = nullptr;
	}
	if(_mainInputListener != nullptr)
	{
		delete _mainInputListener; _mainInputListener = nullptr; 
	}
	if(_renderer != nullptr)
	{
		delete _renderer; _renderer = nullptr;
	}
	if(_logger != nullptr)
	{
		delete _logger; _logger = nullptr;
	}
	if (_physicsMgr != nullptr)
	{
		delete _physicsMgr; _physicsMgr = nullptr;
	}
}

Core* Core::getSingletonPtr( void )
{
	ASSERT(ms_Singleton != NULL);
	return ms_Singleton;
}

Core& Core::getSingleton( void )
{
	ASSERT(ms_Singleton != NULL);
	return *ms_Singleton;
}

void Core::initialise( std::string logfile, bool logToConsole, Vector2 physicsGravity )
{
	if(logfile.empty())
	{
		_logger = new Logger();		
	}
	else
	{
		_logger = new Logger(logfile, logToConsole);
	}

	_renderer = new Renderer();
	_physicsMgr = new PhysicsManager(physicsGravity);
	_mainInputListener = new MainInputListener();

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
	_window->addInputListener(_mainInputListener);

	return _window;
}

void Core::startLoop()
{
	bool done = false;
	clock_t start, end;
	ulong ellapsedTime = 0;

	ulong fpsloopCount = 0;
	ulong fpstimeacum = 0;
	float avgframetime = 0;

	while(!done)
	{
		// DO NOT put any code before this line since it will not be counted as "frame time"
		start = clock();

#pragma region Print FPS
		fpsloopCount++;
		fpstimeacum += ellapsedTime;
		if(fpstimeacum >= 1000)
		{			
			avgframetime = (float)fpstimeacum/(float)fpsloopCount;
			std::cout << " AVG FPS: " << 1000/avgframetime 
				<< " AVG FRAME TIME (ms): " <<  avgframetime << std::endl;
			fpstimeacum = 0;
			fpsloopCount = 0;
		}	
#pragma endregion

		_physicsMgr->Update(ellapsedTime);
		
		if(!_window->updateWindow())
		{
			std::cout << "Window closing" << std::endl;
			done = true;
		}	

		// DO NOT put any code after this line since it will not be counted as "frame time"
		end = clock();
		// compute ellapsed time in milliseconds
		ellapsedTime = (end - start) * 1000 / CLOCKS_PER_SEC;
	}	
}

ApplicationWindow* Core::getWindow() const
{
	return _window;
}



}
