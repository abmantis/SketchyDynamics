
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

	_renderer->addPolygon(b, b->_id);

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
		
	_renderer->removePolygon(b);
	
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
	_renderer->addPolygon(j, bodyid, RQT_Scene);
	return j;
}

void PhysicsManager::destroyJoint( PhysicsJoint* joint, bool destroyB2DJoint /*= true*/ )
{
	_physicsJoints.remove(joint);
	_renderer->removePolygon(joint);

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
		
		uint subPolyCount = b->getSubPolygonCount();
		for (uint i = 1; i < subPolyCount; i += 2)
		{
			b->getSubPolygon(i)->SetMaterial(b->_selectedMaterial);
		}

		b->_selected = true;
		_selectedBodies.push_back(b);
		selectConnectedBodiesRecurse(b);
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

		uint subPolyCount = b->getSubPolygonCount();
		for (uint i = 1; i < subPolyCount; i += 2)
		{
			b->getSubPolygon(i)->SetMaterial(b->_lineMaterial);
		}

		b->_selected = false;
		_selectedBodies.remove(b);
		unselectConnectedBodiesRecurse(b);
	}
}

void PhysicsManager::selectConnectedBodiesRecurse( PhysicsBody *b )
{	
	PhysicsBody *otherPhysicsBody = nullptr;
	for (b2JointEdge* jointEdge = b->_body->GetJointList(); jointEdge != NULL; jointEdge = jointEdge->next)
	{
		otherPhysicsBody = static_cast<PhysicsBody*>(jointEdge->other->GetUserData());
		if(otherPhysicsBody->_selected == false)
		{
			selectBody(otherPhysicsBody);
			selectConnectedBodiesRecurse(otherPhysicsBody);
		}
	}
}

void PhysicsManager::unselectConnectedBodiesRecurse( PhysicsBody *b )
{	
	PhysicsBody *otherPhysicsBody = nullptr;
	for (b2JointEdge* jointEdge = b->_body->GetJointList(); jointEdge != NULL; jointEdge = jointEdge->next)
	{
		otherPhysicsBody = static_cast<PhysicsBody*>(jointEdge->other->GetUserData());
		if(otherPhysicsBody->_selected == true)
		{
			unselectBody(otherPhysicsBody);
			unselectConnectedBodiesRecurse(otherPhysicsBody);
		}
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
	AABB polyAABB;

	PhysicsBodyList::const_iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::const_iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		polyAABB = (*it)->getWorldAABB(true);
		aabb.update(polyAABB);
	}

	return aabb;
}





} // namespace PhySketch