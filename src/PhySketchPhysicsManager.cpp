
#include "PhySketchPhysicsManager.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"

namespace PhySketch
{

template<> PhysicsManager* Singleton<PhysicsManager>::ms_Singleton = 0;

PhysicsManager::PhysicsManager(Vector2 gravity) :
	_physicsBodiesIDSeed(0),
	_physicsJointsIDSeed(0),
	_simulationPaused	(false)
{
	_renderer = Renderer::getSingletonPtr();
	_physicsWorld = new b2World(b2Vec2((float32)gravity.x, (float32)gravity.y));
}

PhysicsManager::~PhysicsManager()
{
	// destroy joints
	PhysicsJointList::iterator jointsItEnd = _physicsJoints.end();
	PhysicsJointList::iterator jointIt = _physicsJoints.begin();
	PhysicsJoint *pj = nullptr;
	while(jointIt != jointsItEnd)
	{
		pj = *jointIt;
		++jointIt;
		destroyJoint(pj, false);
	}
	
	// destroy bodies
	PhysicsBodyList::iterator bodiesItEnd = _physicsBodies.end();
	PhysicsBodyList::iterator bodyIt = _physicsBodies.begin(); 
	PhysicsBody *pb = nullptr;
	while(bodyIt != bodiesItEnd)
	{
		pb = *bodyIt;
		++bodyIt;
		destroyBody(pb, false);
	}

	if(_physicsWorld != nullptr)
	{
		delete _physicsWorld; _physicsWorld = nullptr;
	}
}

PhysicsManager* PhysicsManager::getSingletonPtr( void )
{
	PHYSKETCH_ASSERT(ms_Singleton != NULL);
	return ms_Singleton;
}

PhysicsManager& PhysicsManager::getSingleton( void )
{
	PHYSKETCH_ASSERT(ms_Singleton != NULL);
	return *ms_Singleton;
}

PhysicsBody* PhysicsManager::createBody( b2Body *b2d_body )
{
	// Create new PhysicsBody
	PhySketch::PhysicsBody *b = new PhySketch::PhysicsBody(b2d_body, ++_physicsBodiesIDSeed);

	_physicsBodies.push_back(b);

	// add the body's polygons to the renderer
	if(b->_fillPolygon)
		_renderer->addPolygon(b->_fillPolygon, b->_id);
	if(b->_linePolygon)
		_renderer->addPolygon(b->_linePolygon, b->_id);
	// we already added the polygons to the render so put the flag to false
	b->_needsPolygonUpdate = false;
	
	return b;
}

PhysicsBody* PhysicsManager::createBody( const b2BodyDef& b2d_body_def )
{
	b2Body *b2body = _physicsWorld->CreateBody(&b2d_body_def);
	return createBody(b2body);
}

void PhysicsManager::destroyBody( PhysicsBody *b, bool destroyB2DBody /*= true*/ )
{
	// remove the body from the bodies' list
	_physicsBodies.remove(b);

	// remove all body's polygons from the renderer
	int polygonToRemoveCount = b->_oldPolygons.size();
	for(int i = 0; i < polygonToRemoveCount; i++)
	{
		_renderer->removePolygon(b->_oldPolygons[i]);
	}
	_renderer->removePolygon(b->_fillPolygon);
	_renderer->removePolygon(b->_linePolygon);

	if(destroyB2DBody)
	{
		// destroy the box2d body
		_physicsWorld->DestroyBody(b->_body);
	}

	// delete the body and clear the pointer
	delete b;
	b = nullptr;
}

PhysicsJoint* PhysicsManager::createJoint( b2Joint *b2d_joint, PhysicsJointRepresentation representation )
{
	PhysicsJoint *j = new PhysicsJoint(b2d_joint, representation, Material(Color(1.0f, 0.3f, 0.3f, 0.0f)), ++_physicsJointsIDSeed);
	_physicsJoints.push_back(j);

	// get the ID from the body A
	ulong bodyid = (static_cast<PhysicsBody*>(b2d_joint->GetBodyA()->GetUserData()))->_id;
	_renderer->addPolygon(j->_polygon, bodyid, RQT_Scene);
	return j;
}

void PhysicsManager::destroyJoint( PhysicsJoint* joint, bool destroyB2DJoint /*= true*/ )
{
	_physicsJoints.remove(joint);
	_renderer->removePolygon(joint->_polygon);

	if(destroyB2DJoint)
	{
		_physicsWorld->DestroyJoint(joint->_joint);
	}

	delete joint;
	joint = nullptr;
}

void PhysicsManager::update( ulong advanceTime )
{	

	if(!_simulationPaused)
	{
		stepPhysics(advanceTime);
	}

	//////////////////////////////////////////////////////////////////////////
	// update bodies
	{
		PhysicsBody *pb = nullptr;
		Polygon *poly = nullptr;
		int polygonCount = 0;
		int polygonToRemoveCount = 0;
		int i = 0;
		PhysicsBodyList::iterator itEnd = _physicsBodies.end();
		for (PhysicsBodyList::iterator it = _physicsBodies.begin(); it != itEnd; ++it)
		{
			pb = *it;

			if(pb->_needsPolygonUpdate)
			{
				pb->_needsPolygonUpdate = false;
				polygonToRemoveCount = pb->_oldPolygons.size();
				for(i = 0; i < polygonToRemoveCount; i++)
				{
					poly = pb->_oldPolygons[i];
					_renderer->removePolygon(poly, RQT_Scene);
					delete poly;
					poly = nullptr;
				}

				_renderer->addPolygon(pb->_fillPolygon, pb->_id);
				_renderer->addPolygon(pb->_linePolygon, pb->_id);
				
			}
			pb->update();
		}	
	}

	//////////////////////////////////////////////////////////////////////////
	// update joints
	{
		PhysicsJointList::iterator itEnd = _physicsJoints.end();
		for (PhysicsJointList::iterator it = _physicsJoints.begin(); it != itEnd; ++it)
		{
			(*it)->update();
		}	
	}
}

void PhysicsManager::stepPhysics( ulong ellapsedMillisec )
{

	const ulong millistep = 16;
	const float timeStep = float(millistep)/1000; // 16 milliseconds ~= 60Hz
	const int velIterations = 8;
	const int posIterations = 3;
	static ulong acumulator = 0;
	acumulator += ellapsedMillisec;
	while(acumulator>=millistep)
	{
		_physicsWorld->Step(timeStep, velIterations, posIterations);
		acumulator-=millistep;
	}
}

b2World* PhysicsManager::getPhysicsWorld() const
{
	return _physicsWorld;
}

void PhysicsManager::pauseSimulation()
{
	_simulationPaused = true;
}

void PhysicsManager::playSimulation()
{
	_simulationPaused = false;
}

void PhysicsManager::toggleSimulation()
{
	_simulationPaused = !_simulationPaused;
}

bool PhysicsManager::isSimulationPaused() const
{
	return _simulationPaused;
}

void PhysicsManager::selectBody( PhysicsBody *b )
{
	if(b->_selectable == true && b->_selected == false)
	{
		// Deactivate physics of the selected body 
		b2Body *b2d_body = b->_body;		
		b2d_body->SetAwake(false);
		b2d_body->SetActive(false);	

		b->_selected = true;
		b->_linePolygon->SetMaterial(b->_selectedMaterial);
		_selectedBodies.push_back(b);
	}
}

void PhysicsManager::unselectBody( PhysicsBody *b )
{
	if(b->_selected)
	{
		// Re-activate physics of the selected body 
		b2Body *b2d_body = b->_body;		
		b2d_body->SetAwake(true);
		b2d_body->SetActive(true);	

		b->_selected = false;
		b->_linePolygon->SetMaterial(b->_lineMaterial);

		_selectedBodies.remove(b);
	}
}

void PhysicsManager::setUnselectableBody( PhysicsBody *b )
{
	b->_selectable = false;
	unselectBody(b);
}

void PhysicsManager::setSelectableBody( PhysicsBody *b )
{
	b->_selectable = true;
}

void PhysicsManager::unselectAllBodies()
{
	while(!_selectedBodies.empty())
	{
		unselectBody(_selectedBodies.front());
	}
	
}

void PhysicsManager::translateSelectedBodies( Vector2 translation )
{
	PhysicsBodyList::iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		(*it)->translate(translation);
	}
}

void PhysicsManager::rotateSelectedBodies( float angle, Vector2 rotationCenter )
{
	PhysicsBodyList::iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		(*it)->rotateAroundPoint(angle, rotationCenter);
	}
}

void PhysicsManager::scaleSelectedBodies( Vector2 factor )
{
	PhysicsBodyList::iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		(*it)->scale(factor);
	}
}


const PhysicsManager::PhysicsBodyList& PhysicsManager::getSelectedBodies() const
{
	return _selectedBodies;
}

void PhysicsManager::setActiveOnSelectedBodies( bool flag )
{
	PhysicsBodyList::iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		(*it)->_body->SetActive(flag);
	}
}

void PhysicsManager::setAwakeOnSelectedBodies( bool flag )
{
	PhysicsBodyList::iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		(*it)->_body->SetAwake(flag);
	}
}

PhySketch::AABB PhysicsManager::getSelectedBodiesAABB() const
{
	AABB aabb;
	AABB bodyAABB;

	PhysicsBodyList::const_iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::const_iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		bodyAABB = (*it)->_fillPolygon->getWorldAABB(true);
		aabb.update(bodyAABB);
	}

	return aabb;
}





} // namespace PhySketch