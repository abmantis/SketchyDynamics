
#include "PhySketchApplicationWindow.h"
#include "PhySketchLogger.h"

namespace PhySketch
{
ApplicationWindow::ApplicationWindow( void )
{
	_fullscreen = false; 
	_closePending = false;
	_active = false;
}

ApplicationWindow::~ApplicationWindow( void )
{
}

void ApplicationWindow::addInputListener( InputListener * inputListener )
{
	_inputListeners.insert(inputListener);
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

void ApplicationWindow::callInputListenersMouseDown( MouseButton button, int x, int y )
{
	for(std::set<InputListener*>::iterator iter = _inputListeners.begin(); 
		iter != _inputListeners.end(); iter++)
	{
		(*iter)->mouseDown(button, x, y);
	}
}

void ApplicationWindow::callInputListenersMouseUp( MouseButton button, int x, int y )
{
	for(std::set<InputListener*>::iterator iter = _inputListeners.begin(); 
		iter != _inputListeners.end(); iter++)
	{
		(*iter)->mouseUp(button, x, y);
	}
}

void ApplicationWindow::callInputListenersMouseMoved( int x, int y )
{
	for(std::set<InputListener*>::iterator iter = _inputListeners.begin(); 
		iter != _inputListeners.end(); iter++)
	{
		(*iter)->mouseMoved(x, y);
	}
}


} // namespace PhySketch