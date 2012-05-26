#include "PhySketchInputListener.h"

namespace PhySketch
{


MainInputListener::MainInputListener() : InputListener()
{
	_isLeftMouseDown = false;
	_scribble = nullptr;
}

MainInputListener::~MainInputListener()
{
	if(_scribble)
	{
		delete _scribble;
		_scribble = nullptr;
	}
}

void MainInputListener::keyDown( Key key )
{
}

void MainInputListener::keyUp( Key key )
{
}

void MainInputListener::mouseDown( MouseButton button, int x, int y )
{
	switch(button)
	{
	case MB_Left:
		{
			_isLeftMouseDown = true;
			_scribble = new CIScribble();
			_scribble->addStroke(new CIStroke());
			break;
		}
	case MB_Middle:
		{
			break;
		}
	case MB_Right:
		{
			break;
		}
	}
}

void MainInputListener::mouseUp( MouseButton button, int x, int y )
{
	switch(button)
	{
	case MB_Left:
		{
			_isLeftMouseDown = false;
			if(_scribble)
			{
				delete _scribble;
				_scribble = nullptr;
			}
			break;
		}
	case MB_Middle:
		{
			break;
		}
	case MB_Right:
		{
			break;
		}
	}
	
}

void MainInputListener::mouseMoved( int x, int y )
{
	if(_isLeftMouseDown)
	{
		(*_scribble->getStrokes())[0]->addPoint(x, y);
	}
}


}

