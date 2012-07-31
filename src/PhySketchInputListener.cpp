#include "PhySketchInputListener.h"
#include "PhySketchVector2.h"
#include "PhySketchPolygon.h"
#include "PhySketchRenderer.h"
#include "PhySketchCore.h"
#include "PhySketchPhysicsManager.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPhysicsQueryCallback.h"
#include "PhySketchUtils.h"
#include "PhySketchLogger.h"
#include "PhySketchPhysicsJoint.h"
#include "PhySketchMaterialManager.h"

#include "CALI\CIRecognizer.h"
#include "CALI\CIScribble.h"

#include <Box2D\Dynamics\Joints\b2Joint.h>
#include <Box2D\Collision\b2Collision.h>


namespace PhySketch
{
MainInputListener::MainInputListener() : 
	InputListener		(),
	_isLeftMouseDown	(false),
	_caliStroke			(nullptr),
	_caliScribble		(nullptr),
	_gesturePolygon		(nullptr),
	_gestureSubPolygon	(nullptr),
	_caliRecognizer		(new CIRecognizer()),
	_interactionState	(IS_NONE),
	_gestureMaterial	(nullptr),
	_insideDestructionArea(false)
{
}

MainInputListener::~MainInputListener()
{
	delete _caliScribble;	
	_caliScribble = nullptr;
	//delete _caliStroke; //the scribble already deletes this
	_caliStroke = nullptr;

	delete _caliRecognizer;
	_caliRecognizer = nullptr;

	if(_gesturePolygon)
	{
		_renderer->removePolygon(_gesturePolygon);
		delete _gesturePolygon;
		_gesturePolygon = nullptr;
		_gestureSubPolygon = nullptr;
	}
	if(_transformCenterIndicator)
	{
		_renderer->removePolygon(_transformCenterIndicator);
		delete _transformCenterIndicator;
		_transformCenterIndicator = nullptr;
	}
	
	if(_transformLineIndicator)
	{
		_renderer->removePolygon(_transformLineIndicator);
		delete _transformLineIndicator;
		_transformLineIndicator = nullptr;
	}
	if(_selectedBodiesAABBPoly)
	{
		_renderer->removePolygon(_selectedBodiesAABBPoly);
		delete _selectedBodiesAABBPoly;
		_selectedBodiesAABBPoly = nullptr;
	}
	if(_destructionArea)
	{
		_renderer->removePolygon(_destructionArea);
		delete _destructionArea;
		_destructionArea = nullptr;
	}
}

void MainInputListener::init()
{
	_renderer = Renderer::getSingletonPtr();
	_physicsMgr = PhysicsManager::getSingletonPtr();

	_transformCenterIndicator = new Polygon(VV_Static, "PS_transform_center_indicator");
	_transformCenterIndicator->CreateCircleSubPolygon(DM_TRIANGLE_FAN, Vector2::ZERO_XY, 0.05f, 80);
	_transformCenterIndicator->setVisible(false);
	_renderer->addPolygon(_transformCenterIndicator, RQT_UI);

	_transformLineIndicator = new Polygon(VV_Static, "PS_transform_line_indicator");
	_transformLineIndicator->setVisible(false);
	SubPolygon *tranfLineIndicSubPoly = _transformLineIndicator->createSubPolygon(DM_LINES);
	tranfLineIndicSubPoly->addVertex(Vector2::ZERO_XY);
	tranfLineIndicSubPoly->addVertex(Vector2::UNIT_X);
	_renderer->addPolygon(_transformLineIndicator, RQT_UI);

	_selectedBodiesAABBPoly = new Polygon(VV_Static, "PS_selected_bodies_aabb");
	_selectedBodiesAABBPoly->CreateSquareSubPolygon(DM_LINE_LOOP);
	_selectedBodiesAABBPoly->setVisible(false);
	_renderer->addPolygon(_selectedBodiesAABBPoly, RQT_UI);

	Material* destructAreaMat = MaterialManager::getSingletonPtr()->createMaterial("PS_destruction_area", "../../../textures/destruct_area.png");	
	_destructionArea = new AnimatedPolygon(VV_Static, "PS_destruction_area");
	_destructionArea->CreateSquareSubPolygon(DM_TRIANGLE_FAN);
	_destructionArea->setScale(Vector2(14.0f, 0.7f));
	((Polygon*)_destructionArea)->setPosition(Vector2(0.0f, 4.85f));
//	_destructionArea->setVisible(false);
	_destructionArea->setMaterial(destructAreaMat);
	_renderer->addPolygon(_destructionArea, RQT_UI);

	_gestureMaterial = MaterialManager::getSingletonPtr()->createMaterial("PS_gesture_material", Color(0.0f, 0.0f, 8.0f, 1.0f));
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
				{
					FirstObjectSceneQueryCallback callback(sceneMousePos);
					_renderer->queryScene(sceneMousePos, &callback);
					if(callback._firstPolygon != nullptr)
					{
						PhysicsBody *pb = dynamic_cast<PhysicsBody*>(callback._firstPolygon);
						if( pb != nullptr)
						{
							if(pb->isSelectable())
							{								
								_physicsMgr->selectBody(pb);
								_interactionState = IS_SELECTING_BODIES;
							}
						}
						else 
						{
							// If it is not a body, is it a joint?							
							if( callback._firstPolygon->getUserType() == PHYSKETCH_POLYGON_UTYPE_PHYJOINT)
							{
								PhysicsJoint *pj = static_cast<PhysicsJoint*>(callback._firstPolygon->getUserData());
								if(pj->isSelectable())
								{
									_physicsMgr->selectJoint(pj);
									_interactionState = IS_SELECTING_JOINTS;
								}
							}
						}
					}
					break;
				}
				case IS_SELECTING_BODIES:
				{
					bool onSelectableBody = false;
						
					FirstObjectSceneQueryCallback callback(sceneMousePos);
					_renderer->queryScene(sceneMousePos, &callback);
					if(callback._firstPolygon != nullptr)
					{
						PhysicsBody *pb = dynamic_cast<PhysicsBody*>(callback._firstPolygon);
						if( pb != nullptr)
						{
							if(pb->isSelectable())
							{
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
							
					}

					if(!onSelectableBody)
					{
						// clicked outside a bodie, deselect!
						_physicsMgr->unselectAllBodies();
						_interactionState = IS_NONE;
					}
					else if(_physicsMgr->getSelectedBodies().empty())
					{
						// no more selected objects
						_interactionState = IS_NONE;
					}
						
					break;
				}
				case IS_SELECTING_JOINTS:
				{
					bool onSelectableJoint = false;

					FirstObjectSceneQueryCallback callback(sceneMousePos);
					_renderer->queryScene(sceneMousePos, &callback);
					if(callback._firstPolygon != nullptr)
					{
						if( callback._firstPolygon->getUserType() == PHYSKETCH_POLYGON_UTYPE_PHYJOINT)
						{
							PhysicsJoint *pj = static_cast<PhysicsJoint*>(callback._firstPolygon->getUserData());
							if(pj->isSelectable())
							{
								onSelectableJoint = true;
								if(pj->isSelected())
								{
									_physicsMgr->unselectJoint(pj);
								}
								else
								{
									_physicsMgr->selectJoint(pj);	
								}
							}
						}						

					}

					if(!onSelectableJoint)
					{
						_physicsMgr->unselectAllJoints();
						_interactionState = IS_NONE;
					}
					else if(_physicsMgr->getSelectedJoints().empty())
					{
						// no more selected objects
						_interactionState = IS_NONE;
					}

					break;
				}
				case IS_GESTURING:
					stopDrawingGesture();
					_interactionState = IS_NONE;
					break;				
				case IS_MOVING_JOINTS:
					hideDestructionArea();
					if(_insideDestructionArea)
					{
						highlightDestructionArea(false);
						_physicsMgr->destroySelectedJoints();
					}

					_physicsMgr->validateSelectedJointsAnchors();
					if(_physicsMgr->getSelectedJoints().empty())
					{
						// no more selected objects
						_interactionState = IS_NONE;
					}
					else
					{
						_interactionState = IS_SELECTING_JOINTS;
					}
					break;
				case IS_MOVING_BODIES:
					hideDestructionArea();
					if(_insideDestructionArea)
					{
						highlightDestructionArea(false);
						_physicsMgr->destroySelectedBodies();
					}
					_physicsMgr->validateSelectedJointsAnchors();
					if(_physicsMgr->getSelectedBodies().empty())
					{
						// no more selected objects
						_interactionState = IS_NONE;
					}
					else
					{
						_interactionState = IS_SELECTING_BODIES;
					}
					break;
				case IS_TRANSFORMING_BODIES:
					// Hide AABB and center indicator polygons
					_selectedBodiesAABBPoly->setVisible(false);
					_transformCenterIndicator->setVisible(false);
					_transformLineIndicator->setVisible(false);
					_physicsMgr->validateSelectedJointsAnchors();
					_interactionState = IS_SELECTING_BODIES;
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
				if(position.distanceTo(_lastMousePositions.left) < 3)
				{
					// Make sure the mouse moved more than 3 pixels to remove jitter 
					break;
				}
				
				startDrawingGesture(sceneMousePos);
				_interactionState = IS_GESTURING;
				
				break;
			}		
		case IS_SELECTING_BODIES:
		{
			if(position.distanceTo(_lastMousePositions.left) < 3)
			{
				// Make sure the mouse moved more than 3 pixels to remove jitter
				break;
			}

			FirstObjectSceneQueryCallback callback(_lastMousePositions.leftScene);
			_renderer->queryScene(_lastMousePositions.leftScene, &callback);
			if(callback._firstPolygon != nullptr)
			{
				PhysicsBody *pb = dynamic_cast<PhysicsBody*>(callback._firstPolygon);
				if( pb != nullptr)
				{
					// When in the IS_SELECTING state and the left button 
					// is pressed and the mouse is moved we need to check if it 
					// was moved inside or outside a selected object and change
					// to the IS_MOVING_BODIES or IS_TRANSFORMING_BODIES states accordingly				
					if(pb->isSelected())
					{	
						showDestructionArea();
						_interactionState = IS_MOVING_BODIES;					
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
						_selectedBodiesAABBPoly->setVisible(true);
					
						_transformCenterIndicator->setPosition(aabbCenter);
						_transformCenterIndicator->setVisible(true);

						_transformLineIndicator->setPosition(aabbCenter);
						_transformLineIndicator->setVisible(true);
						_transformLineIndicator->setScale(Vector2(_initialDistFromSelectedBodiesCenter, 0.0f));
						_transformLineIndicator->setAngle(Vector2::UNIT_X.angleTo(sceneMousePos - aabbCenter));

						_interactionState = IS_TRANSFORMING_BODIES;
					}
				}
			}
			break;
		}
		case IS_SELECTING_JOINTS:
		{
			if(position.distanceTo(_lastMousePositions.left) < 3)
			{
				// Make sure the mouse moved more than 3 pixels to remove jitter
				break;
			}

			FirstObjectSceneQueryCallback callback(sceneMousePos);
			_renderer->queryScene(sceneMousePos, &callback);
			if(callback._firstPolygon != nullptr)
			{
				if( callback._firstPolygon->getUserType() == PHYSKETCH_POLYGON_UTYPE_PHYJOINT)
				{
					PhysicsJoint *pj = static_cast<PhysicsJoint*>(callback._firstPolygon->getUserData());
					// When in the IS_SELECTING_JOINT state and the left button 
					// is pressed and the mouse is moved we need to check if it 
					// was moved inside the selected joint
					if(pj->isSelected())
					{	
						showDestructionArea();
						_interactionState = IS_MOVING_JOINTS;					
					}						
				}
			}
			break;
		}
		case IS_GESTURING:
		{
			_caliStroke->addPoint(sceneMousePos.x, sceneMousePos.y);
			_gestureSubPolygon->addVertex(sceneMousePos);
			break;
		}
		case IS_MOVING_BODIES:
		{
			Vector2 translation = sceneMousePos - _lastMousePositions.leftScene;
			_lastMousePositions.leftScene = sceneMousePos;
			_physicsMgr->translateSelectedBodies(translation);
			bool newInsideDestructArea =_destructionArea->isPointInside(sceneMousePos);
			if(!_insideDestructionArea && newInsideDestructArea)
			{
				_insideDestructionArea = true;
				highlightDestructionArea(true);
			}
			else if(_insideDestructionArea && !newInsideDestructArea)
			{
				_insideDestructionArea = false;
				highlightDestructionArea(false);
			}
			break;
		}
		case IS_MOVING_JOINTS:
		{
			Vector2 translation = sceneMousePos - _lastMousePositions.leftScene;
			_lastMousePositions.leftScene = sceneMousePos;
			_physicsMgr->translateSelectedJoints(translation);
			bool newInsideDestructArea =_destructionArea->isPointInside(sceneMousePos);
			if(!_insideDestructionArea && newInsideDestructArea)
			{
				_insideDestructionArea = true;
				highlightDestructionArea(true);
			}
			else if(_insideDestructionArea && !newInsideDestructArea)
			{
				_insideDestructionArea = false;
				highlightDestructionArea(false);
			}
			break;
		}
		case IS_TRANSFORMING_BODIES:
		{
			Vector2 selectedAABBCenter = _selectedBodiesAABB.getCenter();
			Vector2 prevMouseToCenterVec = _lastMousePositions.leftScene - selectedAABBCenter;
			Vector2 currMouseToCenterVec = sceneMousePos - selectedAABBCenter;
			float angle = prevMouseToCenterVec.angleTo(currMouseToCenterVec);
			_physicsMgr->rotateSelectedBodies(angle, selectedAABBCenter);
			_selectedBodiesAABBPoly->rotate(angle);
			_transformLineIndicator->rotate(angle);
			
			float mouseMoveDist = Vector2::distance(_lastMousePositions.leftScene, sceneMousePos);
			float scale = mouseMoveDist / _initialDistFromSelectedBodiesCenter;
			if(scale > 0.5f) scale = 0.5f;	// prevent supa-scaling
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
			_physicsMgr->scaleSelectedBodies(Vector2(scale, scale), selectedAABBCenter);
			_selectedBodiesAABBPoly->scale(Vector2(scale, scale));
			_transformLineIndicator->scale(Vector2(scale, scale));

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
		_gestureSubPolygon = nullptr;
	}
	_gesturePolygon = new Polygon(VV_Stream, "PS_gesture");
	_gestureSubPolygon = _gesturePolygon->createSubPolygon(DM_LINE_STRIP);
	_gestureSubPolygon->setMaterial(_gestureMaterial);
	_gestureSubPolygon->addVertex(startPoint);
	_renderer->addPolygon(_gesturePolygon);
}

void MainInputListener::stopDrawingGesture()
{	
	_caliScribble->addStroke(_caliStroke);
	CIList<CIGesture *>* recGests = _caliRecognizer->recognize(_caliScribble);
	processGesture((*recGests)[0]);
	delete recGests;
	recGests = nullptr;

	_renderer->removePolygon(_gesturePolygon);
	delete _gesturePolygon;
	_gesturePolygon = nullptr;
	_gestureSubPolygon = nullptr;
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
		if(size.x > FLT_MIN && size.y > FLT_MIN)
		{
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
	} 
	else if (gestureName.compare("Circle") == 0 || gestureName.compare("Ellipse") == 0)
	{
		CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
		Vector2 rectP1(static_cast<float>((*enclosingRect)[0].x), static_cast<float>((*enclosingRect)[0].y));
		Vector2 rectP2(static_cast<float>((*enclosingRect)[1].x), static_cast<float>((*enclosingRect)[1].y));
		Vector2 rectP3(static_cast<float>((*enclosingRect)[2].x), static_cast<float>((*enclosingRect)[2].y));

		Vector2 position = _gesturePolygon->getAABB().getCenter();
		Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
		if(size.x > FLT_MIN && size.y > FLT_MIN)
		{
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

	} 	
	else if(gestureName.compare("WavyLine") == 0)
	{
		PhysicsBody *b1 = nullptr;
		PhysicsBody *b2 = nullptr;

		CIPoint p = *(*_caliStroke->getPoints())[0];
		Vector2 firstPt((float)p.x, (float)p.y); 
		p = *(*_caliStroke->getPoints())[_caliStroke->getNumPoints()-1];
		Vector2 lastPt((float)p.x, (float)p.y);

		// Query the b2d world on the zigzag starting poing
		b2AABB aabb;
		Vector2 d(0.00001f, 0.00001f);
		aabb.lowerBound = (firstPt - d).tob2Vec2();
		aabb.upperBound = (firstPt + d).tob2Vec2();
		{
			PhysicsQueryCallback callback(firstPt, true);
			_physicsMgr->getPhysicsWorld()->QueryAABB(&callback, aabb);
			// do we intersect any body?
			if(callback._bodies.size() > 0)
			{
				b1 = *(--callback._bodies.end());
			}
		}
		// Query the b2d world on the zigzag ending poing		
		aabb.lowerBound = (lastPt - d).tob2Vec2();
		aabb.upperBound = (lastPt + d).tob2Vec2();
		PhysicsQueryCallback callback(lastPt, true);
		_physicsMgr->getPhysicsWorld()->QueryAABB(&callback, aabb);
		// do we intersect any body?
		if(callback._bodies.size() > 0)
		{
			b2 = *(--callback._bodies.end());
		}

		if ( b1 && b2 && (b1 != b2) )
		{
			b2DistanceJointDef jd;
			jd.Initialize(b1->getBox2DBody(), b2->getBox2DBody(), firstPt.tob2Vec2(), lastPt.tob2Vec2());
			jd.collideConnected = true;
			jd.frequencyHz = 1.0f;
			jd.dampingRatio = 0.0f;
			b2Joint* j = _physicsMgr->getPhysicsWorld()->CreateJoint(&jd);

			_physicsMgr->createJoint(j);
		}		
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

			_physicsMgr->createJoint(j);
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
			_physicsMgr->createJoint(j);
			return true;
		}
	}

	return false;
}

void MainInputListener::highlightDestructionArea(bool flag)
{
	if(flag)
		_destructionArea->getSubPolygon(0)->getMaterial()->setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	else
		_destructionArea->getSubPolygon(0)->getMaterial()->setColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void MainInputListener::showDestructionArea()
{
	_destructionArea->setPosition(Vector2(0.0f, 4.20f), 3.0f);
}

void MainInputListener::hideDestructionArea()
{
	_destructionArea->setPosition(Vector2(0.0f, 4.85f), 2.0f);
}




}

