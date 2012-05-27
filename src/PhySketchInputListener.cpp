#include "PhySketchInputListener.h"
#include "PhySketchVector2.h"
#include "PhySketchMesh.h"
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
			
			_gestureMesh = new Mesh();
			_gestureMesh->addVertex(position);
			_renderer->addMesh(_gestureMesh);
			
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

			if(_gestureMesh)
			{
				_renderer->removeMesh(_gestureMesh);
				delete _gestureMesh; _gestureMesh = nullptr;
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
		_gestureMesh->addVertex(position);
	}
}


}

