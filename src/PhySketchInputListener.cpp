#include "PhySketchInputListener.h"
#include "PhySketchVector2.h"
#include "PhySketchPolygon.h"
#include "PhySketchRenderer.h"
#include "CALI\CIRecognizer.h"
#include "CALI\CIScribble.h"

namespace PhySketch
{


MainInputListener::MainInputListener() : InputListener()
{
	_renderer = Renderer::getSingletonPtr();
	_isLeftMouseDown = false;
	_caliStroke = nullptr;
	_gesturePolygon = nullptr;	
	_caliRecognizer = new CIRecognizer();
}

MainInputListener::~MainInputListener()
{
	delete _caliStroke;
	_caliStroke = nullptr;

	delete _gesturePolygon;
	_gesturePolygon = nullptr;

	delete _caliRecognizer;
	_caliRecognizer = nullptr;
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

			startDrawingGesture(position);			
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

			stopDrawingGesture();			
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
		_caliStroke->addPoint(position.x, position.y);
		_gesturePolygon->addVertex(position);
	}
}

void MainInputListener::startDrawingGesture( Vector2 startPoint )
{
	delete _caliStroke;
	_caliStroke = new CIStroke();
	_caliStroke->addPoint(startPoint.x, startPoint.y);

	_renderer->removePolygon(_gesturePolygon);
	delete _gesturePolygon;
	_gesturePolygon = new Polygon(Polygon::DM_LINE_STRIP, Polygon::CS_Pixel);
	_gesturePolygon->addVertex(startPoint);
	_renderer->addPolygon(_gesturePolygon);
}

void MainInputListener::stopDrawingGesture()
{
	CIScribble *scribble = new CIScribble();
	scribble->addStroke(_caliStroke);
	CIList<CIGesture *>* recGests = _caliRecognizer->recognize(scribble);
	for (int i = recGests->getNumItems()-1; i >=0; i-- )
	{
		CIGesture *gesture = (*recGests)[i];
		std::cout << gesture->getName() << std::endl;
	}

	delete _caliStroke; _caliStroke = nullptr;

	_renderer->removePolygon(_gesturePolygon);
	delete _gesturePolygon; _gesturePolygon = nullptr;
}


}

