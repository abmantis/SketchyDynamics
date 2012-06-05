#include "PhySketchInputListener.h"
#include "PhySketchVector2.h"
#include "PhySketchPolygon.h"
#include "PhySketchRenderer.h"
#include "CALI\CIRecognizer.h"
#include "CALI\CIScribble.h"
#include "PhySketchBodyPolygon.h"
#include "PhySketchCore.h"

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

			startDrawingGesture(_renderer->windowToScene(position));
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
		position = _renderer->windowToScene(position);
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
	_gesturePolygon = new Polygon(Polygon::DM_LINE_STRIP, Polygon::CS_Scene);
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

		// get "centroid" of the triangle and translate it to origin
		Vector2 position = (rectP1 + rectP2 + rectP3) / 3.0;
		rectP1 -= position;
		rectP2 -= position;
		rectP3 -= position;
		
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set((float)position.x, (float)position.y);
		bodyDef.angle = 0;
		b2Body *body = Core::getSingletonPtr()->getPhysicsWorld()->CreateBody(&bodyDef);

		b2Vec2 vertices[3];
		vertices[0].Set((float)rectP1.x, (float)rectP1.y);
		vertices[1].Set((float)rectP2.x, (float)rectP2.y);
		vertices[2].Set((float)rectP3.x, (float)rectP3.y);

		b2PolygonShape triShape;
		triShape.Set(vertices, 3);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &triShape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.restitution = 0.2f;	
		body->CreateFixture(&fixtureDef);

		poly = new BodyPolygon(Polygon::DM_LINE_LOOP);
		((BodyPolygon*)poly)->setPhysicsBody(body);
	} 
	else if (gesture.compare("Rectangle") == 0 || gesture.compare("Diamond") == 0)
	{
		bValid = true;

		CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
		Vector2 rectP1((*enclosingRect)[0].x, (*enclosingRect)[0].y);
		Vector2 rectP2((*enclosingRect)[1].x, (*enclosingRect)[1].y);
		Vector2 rectP3((*enclosingRect)[2].x, (*enclosingRect)[2].y);
// 		Vector2 rectP4((*enclosingRect)[3].x, (*enclosingRect)[3].y);
						
		Vector2 position =  _gesturePolygon->getAABB().getCenter();
		Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
		Vector2 vectToOrient = rectP2 - rectP1;
		double angle = Vector2::angleBetween(vectToOrient, Vector2::UNIT_X);
		if(vectToOrient.y < 0)
		{
			angle = 180 - angle;
		}

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set((float)position.x, (float)position.y);
		bodyDef.angle = (float)degreesToRadians(angle);
		b2Body *body = Core::getSingletonPtr()->getPhysicsWorld()->CreateBody(&bodyDef);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox((float)size.x*0.5f, (float)size.y*0.5f);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.restitution = 0.2f;	
		body->CreateFixture(&fixtureDef);

		poly = new BodyPolygon(Polygon::DM_LINE_LOOP);
		((BodyPolygon*)poly)->setPhysicsBody(body);
	} 
	else if (gesture.compare("Circle") == 0 || gesture.compare("Ellipse") == 0)
	{
		bValid = true;

		CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
		Vector2 rectP1((*enclosingRect)[0].x, (*enclosingRect)[0].y);
		Vector2 rectP2((*enclosingRect)[1].x, (*enclosingRect)[1].y);
		Vector2 rectP3((*enclosingRect)[2].x, (*enclosingRect)[2].y);
//		Vector2 rectP4((*enclosingRect)[3].x, (*enclosingRect)[3].y);

		Vector2 position = _gesturePolygon->getAABB().getCenter();
		Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set((float)position.x, (float)position.y);
		bodyDef.angle = 0;
		b2Body *body = Core::getSingletonPtr()->getPhysicsWorld()->CreateBody(&bodyDef);

		b2CircleShape circleShape;
		circleShape.m_p.Set(0.0, 0.0);
		circleShape.m_radius = (float)size.x * 0.5f;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circleShape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.restitution = 0.2f;	
		body->CreateFixture(&fixtureDef);

		poly = new BodyPolygon(Polygon::DM_LINE_LOOP);
		((BodyPolygon*)poly)->setPhysicsBody(body);

	} 
// 	else if(gesture.compare("Ellipse") == 0)
// 	{
// 		//bValid = true;
// 	} 
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

