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
	_caliScribble = nullptr;
	_gesturePolygon = nullptr;	
	_caliRecognizer = new CIRecognizer();
}

MainInputListener::~MainInputListener()
{
	delete _caliScribble;	
	_caliScribble = nullptr;
	//delete _caliStroke; //the scribble already deletes this
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
			if(_isLeftMouseDown)
			{
				stopDrawingGesture();	
			}		

			_isLeftMouseDown = false;			
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
	delete _caliScribble;
	_caliScribble = new CIScribble();
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
	_caliScribble->addStroke(_caliStroke);
	CIList<CIGesture *>* recGests = _caliRecognizer->recognize(_caliScribble);
	processGesture((*recGests)[0]->getName());

	_renderer->removePolygon(_gesturePolygon);
}

void MainInputListener::processGesture( std::string gesture )
{
	Polygon *poly;
	bool bValid = false;
	if (gesture.compare("Triangle") == 0)
	{
		bValid = true;

		CIList<CIPoint> *tri = _caliScribble->largestTriangle()->getPoints();
		Vector2 rectP1((*tri)[0].x, (*tri)[0].y);
		Vector2 rectP2((*tri)[1].x, (*tri)[1].y);
		Vector2 rectP3((*tri)[2].x, (*tri)[2].y);

		poly = new Polygon(Polygon::DM_LINE_LOOP, Polygon::CS_Pixel);
		poly->addVertex(rectP1);
		poly->addVertex(rectP2);
		poly->addVertex(rectP3);
	} 
	else if (gesture.compare("Rectangle") == 0 || gesture.compare("Diamond") == 0)
	{
		bValid = true;

		CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
		Vector2 rectP1((*enclosingRect)[0].x, (*enclosingRect)[0].y);
		Vector2 rectP2((*enclosingRect)[1].x, (*enclosingRect)[1].y);
		Vector2 rectP3((*enclosingRect)[2].x, (*enclosingRect)[2].y);
		Vector2 rectP4((*enclosingRect)[3].x, (*enclosingRect)[3].y);

		//poly = new Polygon(Polygon::DM_LINE_LOOP, Polygon::CS_Pixel);
		//poly->addVertex(rectP1);
		//poly->addVertex(rectP2);
		//poly->addVertex(rectP3);
		//poly->addVertex(rectP4);
		
		poly = Polygon::CreateSquare(Polygon::CS_Pixel);
		AABB aabb = _gesturePolygon->getAABB();
		poly->setPosition(aabb.getCenter());
		Vector2 scale(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
		poly->setScale(scale);
		Vector2 vectToOrient = rectP2 - rectP1;
		double angle = Vector2::angleBetween(vectToOrient, Vector2::UNIT_X);
		if(vectToOrient.y < 0)
		{
			angle = 180 - angle;
		}
		poly->setAngle(angle);
	} 
	else if (gesture.compare("Circle") == 0 || gesture.compare("Ellipse") == 0)
	{
		CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
		Vector2 rectP1((*enclosingRect)[0].x, (*enclosingRect)[0].y);
		Vector2 rectP2((*enclosingRect)[1].x, (*enclosingRect)[1].y);
		Vector2 rectP3((*enclosingRect)[2].x, (*enclosingRect)[2].y);
		Vector2 rectP4((*enclosingRect)[3].x, (*enclosingRect)[3].y);

		poly = Polygon::CreateCircle(Polygon::CS_Pixel, Vector2(0,0), 0.5f, 180);
		AABB aabb = _gesturePolygon->getAABB();
		poly->setPosition(aabb.getCenter());
		Vector2 scale(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
		poly->setScale(scale);
		Vector2 vectToOrient = rectP2 - rectP1;
		double angle = Vector2::angleBetween(vectToOrient, Vector2::UNIT_X);
		if(vectToOrient.y < 0)
		{
			angle = 180 - angle;
		}
		poly->setAngle(angle);
		bValid = true;
	} 
	else if(gesture.compare("WavyLine") == 0)
	{
		//bValid = true;
	} 
	else if(gesture.compare("Alpha") == 0)
	{
		//bValid = true;
	} 

	if (bValid)
	{
		_renderer->addPolygon(poly);
	}
}


}

