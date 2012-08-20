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

#include "../dependecies/CALI/CIRecognizer.h"
#include "../dependecies/CALI/CIScribble.h"

#include <Box2D\Dynamics\Joints\b2Joint.h>
#include <Box2D\Collision\b2Collision.h>

#include "../dependecies/poly2tri/poly2tri.h"
#include "PhySketchGuessesList.h"

// Memory leak debug
#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_CLIENTBLOCK
#endif

namespace PhySketch
{

template<> MainInputListener* Singleton<MainInputListener>::ms_Singleton = 0;


MainInputListener::MainInputListener() : 
	InputListener		(),
	_isLeftMouseDown	(false),
	_caliScribble		(nullptr),
	_gesturePolygon		(nullptr),
	_gestureSubPolygon	(nullptr),
	_caliRecognizer		(new CIRecognizer()),
	_interactionState	(IS_NONE),
	_gestureMaterial	(nullptr),
	_insideDestructionArea(false),
	_guessesList		(nullptr),
	_guessesListEnabled	(false),
	_guessesListVisible	(false),
	_lastPhysicsBody	(nullptr),
	_lastPhysicsJoint	(nullptr)
{
}

MainInputListener::~MainInputListener()
{
	delete _caliScribble;	
	_caliScribble = nullptr;	

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
	if(_guessesList)
	{
		delete _guessesList;
		_guessesList = nullptr;
	}
}


MainInputListener* MainInputListener::getSingletonPtr(void)
{
	PHYSKETCH_ASSERT(ms_Singleton != NULL);
	return ms_Singleton;
}

MainInputListener& MainInputListener::getSingleton(void)
{
	PHYSKETCH_ASSERT(ms_Singleton != NULL);
	return *ms_Singleton;
}

void MainInputListener::initInputListener()
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

	_guessesList = new GuessesList();
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
							if( callback._firstPolygon->getUserType() == PHYSKETCH_POLYGON_UTYPE_PHYJOINT)
							{
								PhysicsJoint *pj = static_cast<PhysicsJoint*>(callback._firstPolygon->getUserData());
								if(pj->isSelectable())
								{
									_physicsMgr->selectJoint(pj);
									_interactionState = IS_SELECTING_JOINTS;
								}
							}
							else if( callback._firstPolygon->getUserType() == PHYSKETCH_POLYGON_UTYPE_GUESSESLIST)
							{
								processGuessesListClick(callback._firstPolygon);
							}
						}						
					}

					if(_guessesListVisible)
					{
						hideGuessesList();
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

	if(_guessesListVisible)
	{		
		// If the guesses list is visible, we need to check if the mouse is hovering
		FirstObjectSceneQueryCallback callback(sceneMousePos);
		_renderer->queryScene(sceneMousePos, &callback);
		if(callback._firstPolygon != nullptr && callback._firstPolygon->getUserType() == PHYSKETCH_POLYGON_UTYPE_GUESSESLIST)
		{
			_guessesList->mouseHoverGuess(callback._firstPolygon);
		}
		else
		{
			_guessesList->stopMouseHover();
		}
	}

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
				
				startDrawingGesture(_lastMousePositions.leftScene);
				_interactionState = IS_GESTURING;
				
				break;
			}		
		case IS_SELECTING_BODIES:
		{
			{
				if(position.distanceTo(_lastMousePositions.left) < 3)
				{
					// Make sure the mouse moved more than 3 pixels to remove jitter
					break;
				}

				bool move = false;

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
							move = true;
						}						
					}
					else 
					{							
						if( callback._firstPolygon->getUserType() == PHYSKETCH_POLYGON_UTYPE_PHYJOINT)
						{
							PhysicsJoint *pj = static_cast<PhysicsJoint*>(callback._firstPolygon->getUserData());
							if(pj->isSelected())
							{
								move = true;
							}
						}
					}
				}

				if(move)
				{
					showDestructionArea();
					_interactionState = IS_MOVING_BODIES;
				}
				else
				{					
					_selectedBodiesAABB  = _physicsMgr->getSelectedBodiesAABB();
					Vector2 aabbCenter = _selectedBodiesAABB.getCenter();

					_initialDistFromSelectedBodiesCenter = Vector2::distance(_lastMousePositions.leftScene, aabbCenter);
					_initialDistFromSelectedBodiesCenter = std::max(FLT_MIN, _initialDistFromSelectedBodiesCenter);	
					_currScale = 1.0f;

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
					_transformLineIndicator->setAngle(Vector2::UNIT_X.angleTo(_lastMousePositions.leftScene - aabbCenter));

					_interactionState = IS_TRANSFORMING_BODIES;
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

			FirstObjectSceneQueryCallback callback(_lastMousePositions.leftScene);
			_renderer->queryScene(_lastMousePositions.leftScene, &callback);
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
			
			float currentMouseDist = Vector2::distance(sceneMousePos, selectedAABBCenter);
			float newScale = currentMouseDist / _initialDistFromSelectedBodiesCenter;
			float scale = getScaleFactor(_currScale, newScale);
			_currScale = newScale;

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

	hideGuessesList();
	
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
	std::vector<Vector2> gestVertices, gestSimplifiedVertices;
	_gestureSubPolygon->getOrderedVertices(gestVertices);	
	DouglasPeuckerReduction(gestVertices, 0.005f, gestSimplifiedVertices);
	uint nrVertices = gestSimplifiedVertices.size();
	CIStroke *caliStroke = new CIStroke();
	Vector2 *vert;
	for (uint i = 0; i < nrVertices; ++i)
	{
		vert = &gestSimplifiedVertices[i];
		caliStroke->addPoint(vert->x, vert->y);
	}

	_caliScribble->addStroke(caliStroke);
	CIList<CIGesture *>* recGests = _caliRecognizer->recognize(_caliScribble);

	if(_guessesListEnabled)
	{
		_physicsMgr->_simulationPaused_physketch = true;
		_guessesListVisible = true;
		
		//////////////////////////////////////////////////////////////////////////
		/// Show recognized gestures first and highlighted
		bool validGesture = false;
		int recGestCount = recGests->getNumItems();
		for (int i = 0; i < recGestCount; ++i)
		{
			std::string gestureName = (*recGests)[i]->getName();
			if (gestureName.compare("Triangle") == 0)
			{
				if(createTriangle(true))
				{
					_guessesList->showGuess(GLG_TRIANGLE, true);
					validGesture = true;
				}
			} 
			else if (gestureName.compare("Rectangle") == 0 || gestureName.compare("Diamond") == 0)
			{
				if(createRevoluteJoint(true))
				{
					_guessesList->showGuess(GLG_REVOLUTE, true);
					validGesture = true;
				}
				if(createRectangle(true))
				{
					_guessesList->showGuess(GLG_RECTANGLE, true);
					validGesture = true;
				}				
			} 
			else if (gestureName.compare("Circle") == 0 || gestureName.compare("Ellipse") == 0)
			{
				if(createRevoluteJoint(true))
				{
					_guessesList->showGuess(GLG_REVOLUTE, true);
					validGesture = true;
				}
				if(createCircle(true))
				{
					_guessesList->showGuess(GLG_CIRCLE, true);
					validGesture = true;
				}				
			} 	
			else if(gestureName.compare("WavyLine") == 0)
			{
				if(createSpringJoint(true))
				{
					_guessesList->showGuess(GLG_SPRING, true);
					validGesture = true;
				}
			} 
			else if(gestureName.compare("Alpha") == 0)
			{
				Vector2 intersectPt;
				CIPoint p1, p2, p3, p4;
				static_cast<CIAlpha*>((*recGests)[i])->getIntersectionLines(p1,p2,p3,p4);

				Vector2 v1((float)p1.x, (float)p1.y);
				Vector2 v2((float)p2.x, (float)p2.y);
				Vector2 v3((float)p3.x, (float)p3.y);
				Vector2 v4((float)p4.x, (float)p4.y);		
				if(!lineLineIntersection(v1, v2, v3, v4, intersectPt))
				{
					PHYSKETCH_LOG_WARNING("Can't find alpha intersection?!");
					// Use gesture geometric center instead of intersection point
					intersectPt = _gesturePolygon->getAABB().getCenter();
				}
				if(createWeldJoint(intersectPt, true))
				{
					_guessesList->showGuess(GLG_WELD, true);
					validGesture = true;
				}
			}
		}
		if ( !validGesture )
		{
			if(createFreeform(true))
			{
				_guessesList->showGuess(GLG_FREEFORM, true);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Show all remaining gestures
		if (createRectangle(true))
		{
			_guessesList->showGuess(GLG_RECTANGLE, false);
		}
		if (createTriangle(true))
		{
			_guessesList->showGuess(GLG_TRIANGLE, false);
		}
		if (createCircle(true))
		{
			_guessesList->showGuess(GLG_CIRCLE, false);
		}
		if (createWeldJoint(_gesturePolygon->getAABB().getCenter(), true))
		{
			_guessesList->showGuess(GLG_WELD, false);
		}
		if (createRevoluteJoint(true))
		{
			_guessesList->showGuess(GLG_REVOLUTE, false);
		}
		if (createSpringJoint(true))
		{
			_guessesList->showGuess(GLG_SPRING, false);
		}
		if (createFreeform(true))
		{
			_guessesList->showGuess(GLG_FREEFORM, false);		
		}
		_guessesList->showGuess(GLG_CANCEL, false);
	}

	_lastPhysicsBody = nullptr;
	_lastPhysicsJoint = nullptr;

	// Process the recognized gesture
	processGesture((*recGests)[0]); // TODO: if no gesture was recognized, or the recognized gesture is not context-valid, try with remaining rec. gestures (instead of assuming a freeform after checking the first recognized gesture)

	delete recGests;
	recGests = nullptr;

	_renderer->removePolygon(_gesturePolygon);
}

void MainInputListener::processGesture( CIGesture *gesture )
{
	bool validGesture = false;
	std::string gestureName = gesture->getName();	

	if (gestureName.compare("Triangle") == 0)
	{
		validGesture = createTriangle(false);
	} 
	else if (gestureName.compare("Rectangle") == 0 || gestureName.compare("Diamond") == 0)
	{
		validGesture = createRevoluteJoint(false);
		if(!validGesture)
		{
			validGesture = createRectangle(false);
		}
	} 
	else if (gestureName.compare("Circle") == 0 || gestureName.compare("Ellipse") == 0)
	{
		validGesture = createRevoluteJoint(false);
		if(!validGesture)
		{
			validGesture = createCircle(false);
		}
	} 	
	else if(gestureName.compare("WavyLine") == 0)
	{
		validGesture = createSpringJoint(false);
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
			// Use gesture geometric center instead of intersection point
			intersectPt = _gesturePolygon->getAABB().getCenter();
		}
		validGesture = createWeldJoint(intersectPt, false);
	} 
	if(!validGesture)	// Not recognized -> try free-from
	{	
		createFreeform(false);		
	}

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

bool MainInputListener::createTriangle(bool testOnly)
{
	CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
	Vector2 rectP1(static_cast<float>((*enclosingRect)[0].x), static_cast<float>((*enclosingRect)[0].y));
	Vector2 rectP2(static_cast<float>((*enclosingRect)[1].x), static_cast<float>((*enclosingRect)[1].y));
	Vector2 rectP3(static_cast<float>((*enclosingRect)[2].x), static_cast<float>((*enclosingRect)[2].y));

	Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
	if(size.x > FLT_MIN && size.y > FLT_MIN)
	{
		if(testOnly)
		{
			return true;
		}

		CIList<CIPoint> *tri = _caliScribble->largestTriangle()->getPoints();
		Vector2 triP1(static_cast<float>((*tri)[0].x), static_cast<float>((*tri)[0].y));
		Vector2 triP2(static_cast<float>((*tri)[1].x), static_cast<float>((*tri)[1].y));
		Vector2 triP3(static_cast<float>((*tri)[2].x), static_cast<float>((*tri)[2].y));

		// get "centroid" of the triangle and translate it to origin
		Vector2 position = (triP1 + triP2 + triP3) / 3.0;
		triP1 -= position;
		triP2 -= position;
		triP3 -= position;

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(position.x, position.y);
		bodyDef.angle = 0;
		b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

		b2Vec2 vertices[3];
		vertices[0].Set(triP1.x, triP1.y);
		vertices[1].Set(triP2.x, triP2.y);
		vertices[2].Set(triP3.x, triP3.y);

		b2PolygonShape triShape;
		triShape.Set(vertices, 3);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &triShape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.6f;
		fixtureDef.restitution = 0.3f;	
		body->CreateFixture(&fixtureDef);

		_lastPhysicsBody = _physicsMgr->createBody(body);
		_lastPhysicsBody->setType(PBT_Triangle);

		return true;
	}

	return false;
}

bool MainInputListener::createRectangle(bool testOnly)
{
	CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
	Vector2 rectP1(static_cast<float>((*enclosingRect)[0].x), static_cast<float>((*enclosingRect)[0].y));
	Vector2 rectP2(static_cast<float>((*enclosingRect)[1].x), static_cast<float>((*enclosingRect)[1].y));
	Vector2 rectP3(static_cast<float>((*enclosingRect)[2].x), static_cast<float>((*enclosingRect)[2].y));

	Vector2 position =  _gesturePolygon->getAABB().getCenter();
	Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
	if(size.x > FLT_MIN && size.y > FLT_MIN)
	{
		if(testOnly)
		{
			return true;
		}

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
		fixtureDef.friction = 0.6f;
		fixtureDef.restitution = 0.3f;	
		body->CreateFixture(&fixtureDef);

		_lastPhysicsBody = _physicsMgr->createBody(body);
		_lastPhysicsBody->setType(PBT_Rectangle);

		return true;
	}

	return false;
}

bool MainInputListener::createCircle(bool testOnly)
{
	CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
	Vector2 rectP1(static_cast<float>((*enclosingRect)[0].x), static_cast<float>((*enclosingRect)[0].y));
	Vector2 rectP2(static_cast<float>((*enclosingRect)[1].x), static_cast<float>((*enclosingRect)[1].y));
	Vector2 rectP3(static_cast<float>((*enclosingRect)[2].x), static_cast<float>((*enclosingRect)[2].y));

	Vector2 position = _gesturePolygon->getAABB().getCenter();
	Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
	if(size.x > FLT_MIN && size.y > FLT_MIN)
	{		
		if(testOnly)
		{
			return true;
		}

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
		fixtureDef.friction = 0.6f;
		fixtureDef.restitution = 0.3f;	
		body->CreateFixture(&fixtureDef);

		_lastPhysicsBody = _physicsMgr->createBody(body);
		_lastPhysicsBody->setType(PBT_Circle);

		return true;
	}

	return false;
}

bool MainInputListener::createFreeform(bool testOnly)
{
	std::vector<Vector2> gestVertices, gestSimplifiedVertices;

	_gestureSubPolygon->getOrderedVertices(gestVertices);
	if(gestVertices.size() < 3)
	{
		return false;
	}

	// check first-last point distance
// 	if(gestVertices[0].distanceTo(gestVertices[gestVertices.size()-1]) > 0.5f)
// 	{
// 		return false;
// 	}
				
	DouglasPeuckerReduction(gestVertices, 0.07f, gestSimplifiedVertices);
	if(gestSimplifiedVertices.size() < 3)
	{
		return false;
	}
		
	// Connect first and last vertices for intersection check
	gestSimplifiedVertices.push_back(gestSimplifiedVertices[0]);
	// We cannot have intersections!
	if(checkSegmentSelfIntersection(gestSimplifiedVertices) == false)
	{
		if(testOnly)
		{
			return true;
		}

		// Re-disconnect first and last vertices
		gestSimplifiedVertices.pop_back();

		//translateCentroidTo(gestSimplifiedVertices, Vector2(0.0f, 0.0f));

		// Copy Vector2 array to a p2t::Point array
		std::vector<p2t::Point*> verts;
		uint ptCnt = gestSimplifiedVertices.size();
		verts.reserve(ptCnt);
		Vector2 *vert_p;
		for (uint i = 0; i < ptCnt; ++i)
		{
			vert_p = &gestSimplifiedVertices[i];
			verts.push_back(new p2t::Point( vert_p->x, vert_p->y));
		}			
		/*
		* STEP 1: Create CDT and add primary polyline
		* NOTE: polyline must be a simple polygon. The polyline's points
		* constitute constrained edges. No repeat points!!!
		*/
		p2t::CDT* cdt = new p2t::CDT(verts);
		/*
		* STEP 2: Add holes or Steiner points if necessary
		*/
		/*
		* STEP 3: Triangulate!
		*/
		cdt->Triangulate();
		std::vector<p2t::Triangle*> triangles = cdt->GetTriangles();

		Vector2 position = _gesturePolygon->getAABB().getCenter();
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		//bodyDef.position.Set(position.x, position.y);
		bodyDef.angle = 0;
		b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

		b2Vec2 triVertices[3];			
		uint triCnt = triangles.size();
		for (uint i = 0; i < triCnt; ++i)
		{
			p2t::Triangle& t = *triangles[i];
			p2t::Point& a = *t.GetPoint(0);
			p2t::Point& b = *t.GetPoint(1);
			p2t::Point& c = *t.GetPoint(2);
				
			triVertices[0].Set((float)a.x, (float)a.y);
			triVertices[1].Set((float)b.x, (float)b.y);
			triVertices[2].Set((float)c.x, (float)c.y);

			b2PolygonShape triShape;
			triShape.Set(triVertices, 3);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &triShape;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.6f;
			fixtureDef.restitution = 0.3f;	
			body->CreateFixture(&fixtureDef);
		}
		
		_lastPhysicsBody = _physicsMgr->createBody(body);	
		_lastPhysicsBody->setType(PBT_Freeform);

		// clean up
		delete cdt; 
		cdt = nullptr;

		for (uint i = 0; i < ptCnt; ++i)
		{
			delete verts[i];
		}		

		return true;
	}

	return false;
}

bool MainInputListener::createRevoluteJoint(bool testOnly)
{
	CIList<CIPoint> *enclosingRect = _caliScribble->enclosingRect()->getPoints();
	Vector2 rectP1(static_cast<float>((*enclosingRect)[0].x), static_cast<float>((*enclosingRect)[0].y));
	Vector2 rectP2(static_cast<float>((*enclosingRect)[1].x), static_cast<float>((*enclosingRect)[1].y));
	Vector2 rectP3(static_cast<float>((*enclosingRect)[2].x), static_cast<float>((*enclosingRect)[2].y));

	Vector2 position = _gesturePolygon->getAABB().getCenter();
	Vector2 size(rectP1.distanceTo(rectP2), rectP2.distanceTo(rectP3));
	if(size.x > FLT_MIN && size.y > FLT_MIN && size < Vector2(0.45f, 0.45f))
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
			if(testOnly)
			{
				return true;
			}

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
			
			_lastPhysicsJoint = _physicsMgr->createJoint(j);

			return true;
		}
	}

	return false;	
}

bool MainInputListener::createWeldJoint( Vector2 anchorPoint, bool testOnly )
{
	// Make a small box.
	b2AABB aabb;
	Vector2 d(0.00001f, 0.00001f);
	aabb.lowerBound = (anchorPoint - d).tob2Vec2();
	aabb.upperBound = (anchorPoint + d).tob2Vec2();

	// Query the world for overlapping shapes.
	PhysicsQueryCallback callback(anchorPoint, true);
	_physicsMgr->getPhysicsWorld()->QueryAABB(&callback, aabb);

	// do we intersect at least two bodies?
	if(callback._bodies.size() > 1)
	{
		if(testOnly)
		{
			return true;
		}

		PhysicsBody *b1, *b2;
		std::list<PhysicsBody*>::iterator it = callback._bodies.end();
		--it;
		b1 = *it;
		--it;
		b2 = *it;

		b2WeldJointDef jd;
		jd.Initialize(b1->getBox2DBody(), b2->getBox2DBody(), anchorPoint.tob2Vec2());
		b2Joint* j = _physicsMgr->getPhysicsWorld()->CreateJoint(&jd);

		_lastPhysicsJoint = _physicsMgr->createJoint(j);

		return true;
	}

	return false;
}

bool MainInputListener::createSpringJoint(bool testOnly)
{
	PhysicsBody *b1 = nullptr;
	PhysicsBody *b2 = nullptr;
	CIStroke *calistroke = (*_caliScribble->getStrokes())[0];
	CIPoint p = *(*calistroke->getPoints())[0];
	Vector2 firstPt((float)p.x, (float)p.y); 
	p = *(*calistroke->getPoints())[calistroke->getNumPoints()-1];
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
		if(testOnly)
		{
			return true;
		}

		b2DistanceJointDef jd;
		jd.Initialize(b1->getBox2DBody(), b2->getBox2DBody(), firstPt.tob2Vec2(), lastPt.tob2Vec2());
		jd.collideConnected = true;
		jd.frequencyHz = 1.0f;
		jd.dampingRatio = 0.0f;
		b2Joint* j = _physicsMgr->getPhysicsWorld()->CreateJoint(&jd);

		_lastPhysicsJoint = _physicsMgr->createJoint(j);

		return true;
	}
	return false;
}

void MainInputListener::enableGuessesList( bool enable )
{
	_guessesListEnabled = enable;
}

void MainInputListener::processGuessesListClick( Polygon *clickedPolygon )
{
	if(!_guessesListVisible)
	{
		return;
	}

	GuessesListGuesses selectedGuess = _guessesList->getGuessType(clickedPolygon);

	switch(selectedGuess)
	{
	case GLG_RECTANGLE:
		{
			if(_lastPhysicsJoint != nullptr)
			{
				_physicsMgr->destroyJoint(_lastPhysicsJoint);
				_lastPhysicsJoint = nullptr;
			}
			else if(_lastPhysicsBody != nullptr)
			{
				if (_lastPhysicsBody->getType() == PBT_Rectangle)
				{
					break;
				}
				_physicsMgr->destroyBody(_lastPhysicsBody);
				_lastPhysicsBody = nullptr;
			}
			createRectangle(false);
			break;
		}
	case GLG_TRIANGLE:
		{
			if(_lastPhysicsJoint != nullptr)
			{
				_physicsMgr->destroyJoint(_lastPhysicsJoint);
				_lastPhysicsJoint = nullptr;
			}
			else if(_lastPhysicsBody != nullptr)
			{
				if (_lastPhysicsBody->getType() == PBT_Triangle)
				{
					break;
				}
				_physicsMgr->destroyBody(_lastPhysicsBody);
				_lastPhysicsBody = nullptr;
			}
			createTriangle(false);
			break;
		}
	case GLG_CIRCLE:
		{
			if(_lastPhysicsJoint != nullptr)
			{
				_physicsMgr->destroyJoint(_lastPhysicsJoint);
				_lastPhysicsJoint = nullptr;
			}
			else if(_lastPhysicsBody != nullptr)
			{
				if (_lastPhysicsBody->getType() == PBT_Circle)
				{
					break;
				}
				_physicsMgr->destroyBody(_lastPhysicsBody);
				_lastPhysicsBody = nullptr;
			}
			createCircle(false);
			break;
		}
	case GLG_FREEFORM:
		{
			if(_lastPhysicsJoint != nullptr)
			{
				_physicsMgr->destroyJoint(_lastPhysicsJoint);
				_lastPhysicsJoint = nullptr;
			}
			else if(_lastPhysicsBody != nullptr)
			{
				if (_lastPhysicsBody->getType() == PBT_Freeform)
				{
					break;
				}
				_physicsMgr->destroyBody(_lastPhysicsBody);
				_lastPhysicsBody = nullptr;
			}
			createFreeform(false);
			break;
		}
	case GLG_WELD:
		{
			if(_lastPhysicsJoint != nullptr)
			{
				if(_lastPhysicsJoint->getType() == PJT_Weld)
				{
					break;
				}
				_physicsMgr->destroyJoint(_lastPhysicsJoint);
				_lastPhysicsJoint = nullptr;
			}
			else if(_lastPhysicsBody != nullptr)
			{
				_physicsMgr->destroyBody(_lastPhysicsBody);
				_lastPhysicsBody = nullptr;
			}
			createWeldJoint(_gesturePolygon->getAABB().getCenter(), false);
			break;
		}
	case GLG_REVOLUTE:
		{
			if(_lastPhysicsJoint != nullptr)
			{
				if(_lastPhysicsJoint->getType() == PJT_Revolute)
				{
					break;
				}
				_physicsMgr->destroyJoint(_lastPhysicsJoint);
				_lastPhysicsJoint = nullptr;
			}
			else if(_lastPhysicsBody != nullptr)
			{
				_physicsMgr->destroyBody(_lastPhysicsBody);
				_lastPhysicsBody = nullptr;
			}
			createRevoluteJoint(false);
			break;
		}
	case GLG_ROPE:
		{
			// TODO
			break;
		}
	case GLG_SPRING:
		{
			if(_lastPhysicsJoint != nullptr)
			{
				if(_lastPhysicsJoint->getType() == PJT_Distance)
				{
					break;
				}
				_physicsMgr->destroyJoint(_lastPhysicsJoint);
				_lastPhysicsJoint = nullptr;
			}
			else if(_lastPhysicsBody != nullptr)
			{
				_physicsMgr->destroyBody(_lastPhysicsBody);
				_lastPhysicsBody = nullptr;
			}
			createSpringJoint(false);
			break;
		}
	case GLG_CANCEL:
		{
			if(_lastPhysicsJoint != nullptr)
			{
				_physicsMgr->destroyJoint(_lastPhysicsJoint);
				_lastPhysicsJoint = nullptr;
			}
			else if(_lastPhysicsBody != nullptr)
			{
				_physicsMgr->destroyBody(_lastPhysicsBody);
				_lastPhysicsBody = nullptr;
			}
			break;
		}
	}

	hideGuessesList();
}

void MainInputListener::hideGuessesList()
{
	if(_guessesListVisible)
	{				
		_guessesList->hideGuessesList();
		_guessesListVisible = false;
		_physicsMgr->_simulationPaused_physketch = false;
	}
}


}

