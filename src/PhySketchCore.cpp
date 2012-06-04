#include "PhySketchCore.h"

#ifdef PHYSKETCH_OS_WIN32
	#include "PhySketchApplicationWindow_WGL.h"
#endif
#include "PhySketchLogger.h"
#include "PhySketchRenderer.h"


namespace PhySketch
{
template<> Core* Singleton<Core>::ms_Singleton = 0;

Core::Core( void )
{
	_window = nullptr;
	_logger = nullptr;
	_mainInputListener = nullptr;
	_renderer = nullptr;
	_physicsWorld = nullptr;
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
	if(_physicsWorld != nullptr)
	{
		delete _physicsWorld; _physicsWorld = nullptr;
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
	_window->addInputListener(_mainInputListener);

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

	createPhysicsWorld(physicsGravity);

	_renderer = new Renderer();
	_mainInputListener = new MainInputListener();
	
}

ApplicationWindow* Core::getWindow() const
{
	return _window;
}

b2World* Core::getPhysicsWorld() const
{
	return _physicsWorld;
}

b2World* Core::createPhysicsWorld( Vector2 gravity )
{
	_physicsWorld= new b2World(b2Vec2(gravity.x, gravity.y));
}



}
