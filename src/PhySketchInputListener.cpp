#include "PhySketchInputListener.h"
#include "PhySketchVector2.h"
#include "PhySketchPolygon.h"
#include "PhySketchRenderer.h"

namespace PhySketch
{


MainInputListener::MainInputListener() : InputListener()
{
	_isLeftMouseDown = false;
	_scribble = nullptr;
	_renderer = Renderer::getSingletonPtr();
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

void MainInputListener::mouseDown( MouseButton button, Vector2 position )
{
	switch(button)
	{
	case MB_Left:
		{
			_isLeftMouseDown = true;
			
			_scribble = new CIScribble();
			_scribble->addStroke(new CIStroke());
			(*_scribble->getStrokes())[0]->addPoint(position.x, position.y);
			
			_gesturePolygon = new Polygon();
			_gesturePolygon->addVertex(position);
			_renderer->addPolygon(_gesturePolygon);
			
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

void MainInputListener::mouseUp( MouseButton button, Vector2 position )
{
	switch(button)
	{
	case MB_Left:
		{
			_isLeftMouseDown = false;

			if(_scribble)
			{
				delete _scribble; _scribble = nullptr;
			}

			if(_gesturePolygon)
			{
				_renderer->removePolygon(_gesturePolygon);
				delete _gesturePolygon; _gesturePolygon = nullptr;
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

void MainInputListener::mouseMoved( Vector2 position )
{
	if(_isLeftMouseDown)
	{
		(*_scribble->getStrokes())[0]->addPoint(position.x, position.y);
		_gesturePolygon->addVertex(position);
	}
}


}

