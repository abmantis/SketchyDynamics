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

	createPhysicsWorld(physicsGravity);

	_renderer = new Renderer();
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

	while(!done)
	{
		start = clock();

		stepPhysics(ellapsedTime);

		if(!_window->updateWindow())
		{
			std::cout << "Window closing" << std::endl;
			done = true;
		}	
		end = clock();

		// compute ellapsed time in milliseconds
		ellapsedTime = (end - start) * 1000 / CLOCKS_PER_SEC;
		std::cout << "FPS: " << 1000/ellapsedTime << std::endl;
	}	
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
	_physicsWorld = new b2World(b2Vec2((float32)gravity.x, (float32)gravity.y));
	return _physicsWorld;
}

void Core::stepPhysics( ulong ellapsedMillisec )
{
	
	const ulong millistep = 16;
	const float timeStep = float(millistep)/1000; // 16 milliseconds ~= 60Hz
	const int velIterations = 8;
	const int posIterations = 3;
	static ulong acumulator = 0;
	acumulator += ellapsedMillisec;
	while(acumulator>=millistep)
	{
		_physicsWorld->Step(timeStep, velIterations, posIterations);
		acumulator-=millistep;
	}
}



}
