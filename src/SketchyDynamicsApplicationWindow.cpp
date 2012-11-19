
#include "SketchyDynamicsApplicationWindow.h"
#include "SketchyDynamicsLogger.h"
#include "SketchyDynamicsVector2.h"

namespace SketchyDynamics
{
ApplicationWindow::ApplicationWindow( void )
{
	_fullscreen = false; 
	_closePending = false;
	_active = false;
	_renderer = nullptr;
	_aspectRatio = 0;
}

ApplicationWindow::~ApplicationWindow( void )
{
}

void ApplicationWindow::addInputListener( InputListener * inputListener )
{
	_inputListeners.insert(inputListener);
	inputListener->initInputListener();
}

void ApplicationWindow::removeInputListener( InputListener * inputListener )
{
	_inputListenersToRemove.insert(inputListener);
}

void ApplicationWindow::updateInputListeners()
{
	for(std::set<InputListener*>::iterator iter = _inputListenersToRemove.begin(); 
		iter != _inputListenersToRemove.end(); iter++)
	{
		_inputListeners.erase(*iter);
	}
	_inputListenersToRemove.clear();
}

void ApplicationWindow::callInputListenersKeyDown( Key key )
{
	for(std::set<InputListener*>::iterator iter = _inputListeners.begin(); 
		iter != _inputListeners.end(); iter++)
	{
		(*iter)->keyDown(key);
	}
}

void ApplicationWindow::callInputListenersKeyUp( Key key )
{
	for(std::set<InputListener*>::iterator iter = _inputListeners.begin(); 
		iter != _inputListeners.end(); iter++)
	{
		(*iter)->keyUp(key);
	}
}

void ApplicationWindow::callInputListenersMouseDown( MouseButton button, Vector2 position )
{
	for(std::set<InputListener*>::iterator iter = _inputListeners.begin(); 
		iter != _inputListeners.end(); iter++)
	{
		(*iter)->mouseDown(button, position);
	}
}

void ApplicationWindow::callInputListenersMouseUp( MouseButton button, Vector2 position )
{
	for(std::set<InputListener*>::iterator iter = _inputListeners.begin(); 
		iter != _inputListeners.end(); iter++)
	{
		(*iter)->mouseUp(button, position);
	}
}

void ApplicationWindow::callInputListenersMouseMoved( Vector2 position )
{
	for(std::set<InputListener*>::iterator iter = _inputListeners.begin(); 
		iter != _inputListeners.end(); iter++)
	{
		(*iter)->mouseMoved(position);
	}
}


} // namespace SketchyDynamics