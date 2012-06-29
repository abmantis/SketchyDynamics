#include "PhySketchInputListener.h"
#include "PhySketchVector2.h"
#include "PhySketchPolygon.h"
#include "PhySketchRenderer.h"
#include "CALI\CIRecognizer.h"
#include "CALI\CIScribble.h"
#include "PhySketchCore.h"
#include "PhySketchPhysicsManager.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPhysicsQueryCallback.h"
#include "Box2D\Collision\b2Collision.h"
#include "PhySketchUtils.h"
#include "PhySketchLogger.h"
#include "PhySketchPhysicsJoint.h"
#include "Box2D\Dynamics\Joints\b2Joint.h"

namespace PhySketch
{


MainInputListener::MainInputListener() : InputListener()
{
	_renderer = Renderer::getSingletonPtr();
	_physicsMgr = PhysicsManager::getSingletonPtr();
	_isLeftMouseDown = false;
	_caliStroke = nullptr;
	_caliScribble = nullptr;
	_gesturePolygon = nullptr;	
	_caliRecognizer = new CIRecognizer();
	_interactionState = IS_NONE;

	_transformIndicator = Polygon::CreateCircle(Polygon::DM_TRIANGLE_FAN, Vector2::ZERO_XY, 0.05f, 80, "PS_transform_indicator");
	_selectedBodiesAABBPoly = Polygon::CreateSquare(Polygon::DM_LINE_LOOP, "PS_selected_bodies_aabb");
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

	if(_gesturePolygon)
	{
		_renderer->removePolygon(_gesturePolygon);
		delete _gesturePolygon;
		_gesturePolygon = nullptr;
	}
	if(_transformIndicator)
	{
		_renderer->removePolygon(_transformIndicator);
		delete _transformIndicator;
		_transformIndicator = nullptr;
	}
	if(_selectedBodiesAABBPoly)
	{
		_renderer->removePolygon(_selectedBodiesAABBPoly);
		delete _selectedBodiesAABBPoly;
		_selectedBodiesAABBPoly = nullptr;
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
			_lastMousePositions.left		= position;
			_lastMousePositions.leftScene	= _renderer->windowToScene(position);
			
			break;
		}
	case MB_Middle:
		{
			_lastMousePositions.middle		= position;
			_lastMousePositions.middleScene	= _renderer->windowToScene(position);
			break;
		}
	case MB_Right:
		{
			_lastMousePositions.right		= position;
			_lastMousePositions.rightScene	= _renderer->windowToScene(position);
			break;
		}
	}
}

void MainInputListener::mouseUp( MouseButton button, Vector2 position )
{
	Vector2 sceneMousePos = _renderer->windowToScene(position);
	switch(button)
	{
	case MB_Left:
		{
			if(_isLeftMouseDown)
			{
				switch(_interactionState)
				{
				case IS_NONE:
				case IS_SELECTING:
					{
						bool onSelectableBody = false;
						
						b2AABB aabb;
						Vector2 d(0.00001f, 0.00001f);
						aabb.lowerBound = (sceneMousePos - d).tob2Vec2();
						aabb.upperBound = (sceneMousePos + d).tob2Vec2();
						PhysicsQueryCallback callback(sceneMousePos, true);
						_physicsMgr->setActiveOnSelectedBodies(true);
						_physicsMgr->getPhysicsWorld()->QueryAABB(&callback, aabb);
						_physicsMgr->setActiveOnSelectedBodies(false);
						if(!callback._bodies.empty())
						{
							PhysicsBody *pb = callback._bodies.back();
							if(pb->isSelectable())
							{
								b2Body *b2d_body = pb->getBox2DBody();
								onSelectableBody = true;
								if(pb->isSelected())
								{
									_physicsMgr->unselectBody(pb);
								}
								else
								{
									_physicsMgr->selectBody(pb);	
								}
							}
						}

						if(!onSelectableBody)
						{
							// clicked outside a bodie, deselect!
							_physicsMgr->unselectAllBodies();
							_interactionState = IS_NONE;
						}
						else if(_physicsMgr->getSelectedBodies().empty())
						{
							// no more selected bodies
							_interactionState = IS_NONE;
						}
						else
						{
							// we have at least one body selected
							_interactionState = IS_SELECTING;
						}
						
						break;
					}
				case IS_GESTURING:
					stopDrawingGesture();
					_interactionState = IS_NONE;
					break;
				case IS_MOVING:
					_interactionState = IS_SELECTING;
					break;
				case IS_TRANSFORMING:
					// Remove AABB and center indicator polygons
					_renderer->removePolygon(_selectedBodiesAABBPoly);
					_renderer->removePolygon(_transformIndicator);
					_interactionState = IS_SELECTING;
					break;
				}

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
	Vector2 sceneMousePos = _renderer->windowToScene(position);
	if(_isLeftMouseDown)
	{
		switch(_interactionState)
		{
		case IS_NONE:
			{
				startDrawingGesture(sceneMousePos);
				_interactionState = IS_GESTURING;
				break;
			}
		case IS_GESTURING:
			{
				_caliStroke->addPoint(sceneMousePos.x, sceneMousePos.y);
				_gesturePolygon->addVertex(sceneMousePos);
				break;
			}
		case IS_SELECTING:
		{
			b2AABB aabb;
			Vector2 d(0.00001f, 0.00001f);
			aabb.lowerBound = (sceneMousePos - d).tob2Vec2();
			aabb.upperBound = (sceneMousePos + d).tob2Vec2();
			PhysicsQueryCallback callback(sceneMousePos, true);
			_physicsMgr->setActiveOnSelectedBodies(true);
			_physicsMgr->getPhysicsWorld()->QueryAABB(&callback, aabb);
			_physicsMgr->setActiveOnSelectedBodies(false);
			if(!callback._bodies.empty())
			{
				// When in the IS_SELECTING state and the left button 
				// is pressed and the mouse is moved we need to check if it 
				// was moved inside or outside a selected object and change
				// to the IS_MOVING or IS_TRANSFORMING states accordingly

				PhysicsBody *pb = callback._bodies.back();
				if(pb->isSelected())
				{	
					_interactionState = IS_MOVING;					
				}
				else
				{					
					_selectedBodiesAABB  = _physicsMgr->getSelectedBodiesAABB();
					Vector2 aabbCenter = _selectedBodiesAABB.getCenter();

					_initialDistFromSelectedBodiesCenter = Vector2::distance(sceneMousePos, aabbCenter);
					_initialDistFromSelectedBodiesCenter = std::max(FLT_MIN, _initialDistFromSelectedBodiesCenter);	

					// Show AABB of the selected bodies and AABB center indicator
					_selectedBodiesAABBPoly->setPosition(aabbCenter);
					_selectedBodiesAABBPoly->setScale(_selectedBodiesAABB.getSize());
					_selectedBodiesAABBPoly->setAngle(0.0f);
					_renderer->addPolygon(_selectedBodiesAABBPoly);
					
					_transformIndicator->setPosition(aabbCenter);
					_renderer->addPolygon(_transformIndicator);

					_interactionState = IS_TRANSFORMING;
				}
			}
			break;
		}
		case IS_MOVING:
		{
			Vector2 translation = sceneMousePos - _lastMousePositions.leftScene;
			_lastMousePositions.leftScene = sceneMousePos;
			_physicsMgr->translateSelectedBodies(translation);
			break;
		}
		case IS_TRANSFORMING:
		{
			Vector2 selectedAABBCenter = _selectedBodiesAABB.getCenter();
			Vector2 prevMouseToCenterVec = _lastMousePositions.leftScene - selectedAABBCenter;
			Vector2 currMouseToCenterVec = sceneMousePos - selectedAABBCenter;
			float angle = prevMouseToCenterVec.angleTo(currMouseToCenterVec);
			_physicsMgr->rotateSelectedBodies(angle, selectedAABBCenter);
			_selectedBodiesAABBPoly->rotate(angle);
			
			float mouseMoveDist = Vector2::distance(_lastMousePositions.leftScene, sceneMousePos);
			float scale = mouseMoveDist / _initialDistFromSelectedBodiesCenter;
			if(prevMouseToCenterVec.sqrdLength() < currMouseToCenterVec.sqrdLength())
			{
				// if the mouse is moving away from the AABB center then we need to scale up
				scale += 1.0f;
			}
			else
			{
				// if the mouse is moving closer to the AABB center then we need to scale down
				scale = 1.0f - scale;
			}
			_physicsMgr->scaleSelectedBodies(Vector2(scale, scale));
			//_selectedBodiesAABBPoly->setScale(dist);

			 _lastMousePositions.leftScene = sceneMousePos;

			break;
		}
		}		
	}
}

void MainInputListener::startDrawingGesture( Vector2 startPoint )
{
	delete _caliScribble;
	_caliScribble = new CIScribble();
	_caliStroke = new CIStroke();
	_caliStroke->addPoint(startPoint.x, startPoint.y);

	if(_gesturePolygon !=nullptr)
	{
		_renderer->removePolygon(_gesturePolygon);
		delete _gesturePolygon;
		_gesturePolygon = nullptr;
	}
	_gesturePolygon = new Polygon(Polygon::VV_Stream, Polygon::DM_LINE_STRIP, "PS_gesture");
	_gesturePolygon->addVertex(startPoint);
	_renderer->addPolygon(_gesturePolygon);
}

void MainInputListener::stopDrawingGesture()
{	
	_caliScribble->addStroke(_caliStroke);
	CIList<CIGesture *>* recGests = _caliRecognizer->recognize(_caliScribble);
	processGesture((*recGests)[0]);

	_renderer->removePolygon(_gesturePolygon);
	delete _gesturePolygon;
	_gesturePolygon = nullptr;
}

void MainInputListener::processGesture( CIGesture *gesture )
{
	std::string gestureName = gesture->getName();

	if (gestureName.compare("Triangle") == 0)
	{
		CIList<CIPoint> *tri = _caliScribble->largestTriangle()->getPoints();
		Vector2 rectP1(static_cast<float>((*tri)[0].x), static_cast<float>((*tri)[0].y));
		Vector2 rectP2(static_cast<float>((*tri)[1].x), static_cast<float>((*tri)[1].y));
		Vector2 rectP3(static_cast<float>((*tri)[2].x), static_cast<float>((*tri)[2].y));

		// get "centroid" of the triangle and translate it to origin
		Vector2 position = (rectP1 + rectP2 + rectP3) / 3.0;
		rectP1 -= position;
		rectP2 -= position;
		rectP3 -= position;
		
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(position.x, position.y);
		bodyDef.angle = 0;
		b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

		b2Vec2 vertices[3];
		vertices[0].Set(rectP1.x, rectP1.y);
		vertices[1].Set(rectP2.x, rectP2.y);
		vertices[2].Set(rectP3.x, rectP3.y);

		b2PolygonShape triShape;
		triShape.Set(vertices, 3);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &triShape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.restitution = 0.2f;	
		body->CreateFixture(&fixtureDef);

		_physicsMgr->createBody(body);		
	} 
	else if (gestureName.compare("Rectangle") == 0 || gestureName.compare("Diamond") == 0)
	{
		CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
		Vector2 rectP1(static_cast<float>((*enclosingRect)[0].x), static_cast<float>((*enclosingRect)[0].y));
		Vector2 rectP2(static_cast<float>((*enclosingRect)[1].x), static_cast<float>((*enclosingRect)[1].y));
		Vector2 rectP3(static_cast<float>((*enclosingRect)[2].x), static_cast<float>((*enclosingRect)[2].y));
						
		Vector2 position =  _gesturePolygon->getAABB().getCenter();
		Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
		Vector2 vectToOrient = rectP2 - rectP1;
		float angle = Vector2::angleBetween(vectToOrient, Vector2::UNIT_X);
		if(vectToOrient.y < 0)
		{
			angle = M_PI - angle;
		}

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(position.x, position.y);
		bodyDef.angle = angle;
		b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(size.x*0.5f, size.y*0.5f);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.restitution = 0.2f;	
		body->CreateFixture(&fixtureDef);

		_physicsMgr->createBody(body);
	} 
	else if (gestureName.compare("Circle") == 0)
	{
		CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
		Vector2 rectP1(static_cast<float>((*enclosingRect)[0].x), static_cast<float>((*enclosingRect)[0].y));
		Vector2 rectP2(static_cast<float>((*enclosingRect)[1].x), static_cast<float>((*enclosingRect)[1].y));
		Vector2 rectP3(static_cast<float>((*enclosingRect)[2].x), static_cast<float>((*enclosingRect)[2].y));

		Vector2 position = _gesturePolygon->getAABB().getCenter();
		Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
		
		if(checkForCircleJoint(size, position) == false)
		{
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position.Set(position.x, position.y);
			bodyDef.angle = 0.0f;
			b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

			b2CircleShape circleShape;
			circleShape.m_p.Set(0.0f, 0.0f);
			circleShape.m_radius = size.x * 0.5f;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circleShape;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			fixtureDef.restitution = 0.2f;	
			body->CreateFixture(&fixtureDef);

			_physicsMgr->createBody(body);
		}

	} 
	else if(gestureName.compare("Ellipse") == 0)
	{
		CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
		Vector2 rectP1(static_cast<float>((*enclosingRect)[0].x), static_cast<float>((*enclosingRect)[0].y));
		Vector2 rectP2(static_cast<float>((*enclosingRect)[1].x), static_cast<float>((*enclosingRect)[1].y));
		Vector2 rectP3(static_cast<float>((*enclosingRect)[2].x), static_cast<float>((*enclosingRect)[2].y));

		Vector2 position = _gesturePolygon->getAABB().getCenter();
		Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));

		if(checkForCircleJoint(size, position) == false)
		{
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position.Set(position.x, position.y);
			bodyDef.angle = 0.0f;
			b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

			b2CircleShape circleShape;	//TODO: ellipse and not circle
			circleShape.m_p.Set(0.0f, 0.0f);
			circleShape.m_radius = size.x * 0.5f;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circleShape;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			fixtureDef.restitution = 0.2f;	
			body->CreateFixture(&fixtureDef);

			_physicsMgr->createBody(body);
		}
		
	} 
	else if(gestureName.compare("WavyLine") == 0)
	{
	} 
	else if(gestureName.compare("Alpha") == 0)
	{
		Vector2 intersectPt;
		CIPoint p1, p2, p3, p4;
		static_cast<CIAlpha*>(gesture)->getIntersectionLines(p1,p2,p3,p4);

		Vector2 v1((float)p1.x, (float)p1.y);
		Vector2 v2((float)p2.x, (float)p2.y);
		Vector2 v3((float)p3.x, (float)p3.y);
		Vector2 v4((float)p4.x, (float)p4.y);		
		if(!lineLineIntersection(v1, v2, v3, v4, intersectPt))
		{
			PHYSKETCH_LOG_WARNING("Can't find alpha intersection?!");
			return;
		}

		// Make a small box.
		b2AABB aabb;
		Vector2 d(0.00001f, 0.00001f);
		aabb.lowerBound = (intersectPt - d).tob2Vec2();
		aabb.upperBound = (intersectPt + d).tob2Vec2();

		// Query the world for overlapping shapes.
		PhysicsQueryCallback callback(intersectPt, true);
		_physicsMgr->getPhysicsWorld()->QueryAABB(&callback, aabb);

		// do we intersect at least two bodies?
		if(callback._bodies.size() > 1)
		{
			PhysicsBody *b1, *b2;
			std::list<PhysicsBody*>::iterator it = callback._bodies.end();
			--it;
			b1 = *it;
			--it;
			b2 = *it;

			b2WeldJointDef jd;
			jd.Initialize(b1->getBox2DBody(), b2->getBox2DBody(), intersectPt.tob2Vec2());
			b2Joint* j = _physicsMgr->getPhysicsWorld()->CreateJoint(&jd);

			_physicsMgr->createJoint(j, PJR_Cross);
		}
	} 

}

bool MainInputListener::checkForCircleJoint( Vector2 size, Vector2 position )
{
	if(size < Vector2(0.45f, 0.45f))
	{
		//////////////////////////////////////////////////////////////////////////
		// Since this is a small circle/ellipse, let's check if it intersects two bodies

		// Make a small box.
		b2AABB aabb;
		Vector2 d(0.00001f, 0.00001f);
		aabb.lowerBound = (position - d).tob2Vec2();
		aabb.upperBound = (position + d).tob2Vec2();

		// Query the world for overlapping shapes.
		PhysicsQueryCallback callback(position, true);
		_physicsMgr->getPhysicsWorld()->QueryAABB(&callback, aabb);

		// do we intersect at least two bodies?
		if(callback._bodies.size() > 1)
		{
			// Create a joint on the two latest/closest bodies
			PhysicsBody *b1, *b2;
			std::list<PhysicsBody*>::iterator it = callback._bodies.end();
			--it;
			b1 = *it;
			--it;
			b2 = *it;

			b2RevoluteJointDef jd;
			jd.Initialize(b1->getBox2DBody(), b2->getBox2DBody(), position.tob2Vec2());
			b2Joint* j = _physicsMgr->getPhysicsWorld()->CreateJoint(&jd);
			_physicsMgr->createJoint(j, PJR_Circle);
			return true;
		}
	}

	return false;
}


}

