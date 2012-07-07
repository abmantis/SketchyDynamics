
#include "PhySketchPhysicsManager.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"

namespace PhySketch
{

template<> PhysicsManager* Singleton<PhysicsManager>::ms_Singleton = 0;

PhysicsManager::PhysicsManager(Vector2 gravity) :
	_physicsBodiesIDSeed		(0),
	_physicsJointsIDSeed		(0),
	_simulationPaused			(false),
	_simulationPaused_internal	(false)
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
	_selectedBodies.remove(b);
		
	_renderer->removePolygon(b);
	
	if(destroyB2DBody)
	{
		// destroy the box2d body
		_physicsWorld->DestroyBody(b->_body);
	}

	// delete the body and clear the pointer
	delete b;
	b = nullptr;

	if(_selectedBodies.size() == 0 && _selectedJoints.size() == 0)
	{
		_simulationPaused_internal = false;
	}
}

PhysicsJoint* PhysicsManager::createJoint( b2Joint *b2d_joint )
{
	PhysicsJoint *j = nullptr;
	b2JointType type = b2d_joint->GetType();
	switch (type)
	{
	case e_revoluteJoint:
		{
			b2RevoluteJoint* revj = static_cast<b2RevoluteJoint*>(b2d_joint);
			j = new PhysicsJointRevolute(revj, Material(Color(1.0f, 0.3f, 0.3f, 0.0f)), Material(Color(1.0f, 0.7f, 0.7f, 0.0f)), ++_physicsJointsIDSeed);
			break;
		}
	case e_weldJoint:
		{
			b2WeldJoint* weldj = static_cast<b2WeldJoint*>(b2d_joint);
			j = new PhysicsJointWeld(weldj, Material(Color(1.0f, 0.3f, 0.3f, 0.0f)), Material(Color(1.0f, 0.7f, 0.7f, 0.0f)), ++_physicsJointsIDSeed);
			break;
		}
	default:
		throw std::exception("Unsupported joint type");
		return NULL;
	}

	 
	_physicsJoints.push_back(j);

	// TODO: get the ID from the frontest body
	// get the ID from the body A
	ulong bodyid = (static_cast<PhysicsBody*>(b2d_joint->GetBodyA()->GetUserData()))->_id;
	_renderer->addPolygon(j, bodyid, RQT_Scene);
	return j;
}

void PhysicsManager::destroyJoint( PhysicsJoint* joint, bool destroyB2DJoint /*= true*/ )
{
	_physicsJoints.remove(joint);
	_selectedJoints.remove(joint);
	_renderer->removePolygon(joint);

	if(destroyB2DJoint)
	{
		_physicsWorld->DestroyJoint(joint->_joint);
	}

	delete joint;
	joint = nullptr;

	if(_selectedBodies.size() == 0 && _selectedJoints.size() == 0)
	{
		_simulationPaused_internal = false;
	}
}

void PhysicsManager::update( ulong advanceTime )
{	
	if(!_simulationPaused && !_simulationPaused_internal)
	{
		stepPhysics(advanceTime);
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
	return _simulationPaused || _simulationPaused_internal;
}

void PhysicsManager::selectBody( PhysicsBody *b )
{
	if(b->_selectable == true && b->_selected == false)
	{
		uint subPolyCount = b->getSubPolygonCount();
		for (uint i = 1; i < subPolyCount; i += 2)
		{
			b->getSubPolygon(i)->SetMaterial(b->_selectedMaterial);
		}

		b->_selected = true;
		_selectedBodies.push_back(b);
		
		// put the body in sleep so that forces stop being applied
		b->_body->SetAwake(false);
		selectConnectedBodiesRecurse(b);

		_simulationPaused_internal = true;
	}
}

void PhysicsManager::unselectBody( PhysicsBody *b )
{
	if(b->_selected)
	{
		uint subPolyCount = b->getSubPolygonCount();
		for (uint i = 1; i < subPolyCount; i += 2)
		{
			b->getSubPolygon(i)->SetMaterial(b->_lineMaterial);
		}

		b->_selected = false;
		_selectedBodies.remove(b);

		// wake up the body
		b->_body->SetAwake(true);
		unselectConnectedBodiesRecurse(b);
		
		if(_selectedBodies.size() == 0 && _selectedJoints.size() == 0)
		{
			_simulationPaused_internal = false;
		}
	}
}

void PhysicsManager::selectConnectedBodiesRecurse( PhysicsBody *b )
{	
	PhysicsJoint *phyjoint = nullptr;
	PhysicsBody *otherPhysicsBody = nullptr;	
	for (b2JointEdge* jointEdge = b->_body->GetJointList(); jointEdge != NULL; jointEdge = jointEdge->next)
	{
		otherPhysicsBody = static_cast<PhysicsBody*>(jointEdge->other->GetUserData());
		if(otherPhysicsBody->_selectable == true && otherPhysicsBody->_selected == false)
		{
			selectBody(otherPhysicsBody);
		}
		phyjoint = static_cast<PhysicsJoint*>(jointEdge->joint->GetUserData());
		if(phyjoint->_selectable == true && phyjoint->_selected == false)
		{
			selectJoint(phyjoint);
		}

	}
}

void PhysicsManager::unselectConnectedBodiesRecurse( PhysicsBody *b )
{	
	PhysicsJoint *phyjoint = nullptr;
	PhysicsBody *otherPhysicsBody = nullptr;
	b2JointEdge* nextJointEdge = b->_body->GetJointList(); 
	b2JointEdge* jointEdge = nullptr;
	while (nextJointEdge != NULL)
	{
		jointEdge = nextJointEdge;
		nextJointEdge = nextJointEdge->next;

		phyjoint = static_cast<PhysicsJoint*>(jointEdge->joint->GetUserData());
		otherPhysicsBody = static_cast<PhysicsBody*>(jointEdge->other->GetUserData());
		if(phyjoint->_selected == true)
		{
			unselectJoint(phyjoint);
		}		
		if(otherPhysicsBody->_selected == true)
		{
			unselectBody(otherPhysicsBody);
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

	translateSelectedJoints(translation);
}

void PhysicsManager::rotateSelectedBodies( float angle, Vector2 rotationCenter )
{
	PhysicsBodyList::iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		(*it)->rotateAroundPoint(angle, rotationCenter);
	}

	rotateSelectedJoints(angle, rotationCenter);
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
		polyAABB = (*it)->getTransformedAABB(true);
		aabb.update(polyAABB);
	}

	return aabb;
}

void PhysicsManager::selectJoint( PhysicsJoint *j )
{
	if(j->_selectable == true && j->_selected == false)
	{
		j->select();
		_selectedJoints.push_back(j);
		_simulationPaused_internal = true;
	}
}

void PhysicsManager::unselectJoint( PhysicsJoint *j )
{
	if(j->_selected)
	{
		j->unselect();
		_selectedJoints.remove(j);			
				
		if(_selectedBodies.size() == 0 && _selectedJoints.size() == 0)
		{
			_simulationPaused_internal = false;
		}
	}
}

void PhysicsManager::setUnselectableJoint( PhysicsJoint *j )
{
	j->_selectable = false;
	unselectJoint(j);
}

void PhysicsManager::setSelectableJoint( PhysicsJoint *j )
{
	j->_selectable = true;
}

void PhysicsManager::unselectAllJoints()
{
	while(!_selectedJoints.empty())
	{
		unselectJoint(_selectedJoints.front());
	}
}

const PhysicsManager::PhysicsJointList& PhysicsManager::getSelectedJoints() const
{
	return _selectedJoints;
}

void PhysicsManager::translateSelectedJoints( Vector2 translation )
{
	PhysicsJointList::iterator itEnd = _selectedJoints.end();
	for (PhysicsJointList::iterator it = _selectedJoints.begin(); it != itEnd; ++it)
	{
		(*it)->translate(translation);
	}
}

bool PhysicsManager::validateJointAnchors( PhysicsJoint *j )
{
	JointAnchorsSituation jas = j->checkAnchorsSituation();
	switch(jas)
	{
	case JAS_MOVED:
		{
			// The joint is still inside both bodies, recreate it
			b2Joint* newb2djoint = nullptr;
			b2Joint* oldb2djoint = j->getBox2DJoint();
			b2JointType type = oldb2djoint->GetType();
			switch (type)
			{
			case e_revoluteJoint:
				{
					b2RevoluteJointDef jd;
					b2RevoluteJoint* revj = static_cast<b2RevoluteJoint*>(oldb2djoint);
					jd.Initialize(revj->GetBodyA(), revj->GetBodyB(), j->getPosition().tob2Vec2());
					jd.collideConnected	= revj->GetCollideConnected();
					jd.enableLimit		= revj->IsLimitEnabled();
					jd.enableMotor		= revj->IsMotorEnabled();
					jd.lowerAngle		= revj->GetLowerLimit();
					jd.maxMotorTorque	= revj->GetMaxMotorTorque();
					jd.motorSpeed		= revj->GetMotorSpeed();
					jd.upperAngle		= revj->GetUpperLimit();
					newb2djoint			= _physicsWorld->CreateJoint(&jd);
					break;
				}
			case e_weldJoint:
				{
					b2WeldJointDef jd;
					b2WeldJoint* weldj = static_cast<b2WeldJoint*>(oldb2djoint);
					jd.Initialize(weldj->GetBodyA(), weldj->GetBodyB(), j->getPosition().tob2Vec2());
					jd.collideConnected	= weldj->GetCollideConnected();
					jd.dampingRatio		= weldj->GetDampingRatio();
					jd.frequencyHz		= weldj->GetFrequency();
					newb2djoint			= _physicsWorld->CreateJoint(&jd);
					break;
				}
			}

			PHYSKETCH_ASSERT(newb2djoint && "Joint type not implemented");
			newb2djoint->SetUserData(j);
			j->_joint = newb2djoint;
			_physicsWorld->DestroyJoint(oldb2djoint);
			
			break;
		}	
		case JAS_MOVED_OUT:
		{
			destroyJoint(j, true);
			j = nullptr;
			return false;
			break;
		}
	}

	// the joint still exists
	return true;
}

void PhysicsManager::validateSelectedJointsAnchors()
{
	PhysicsJointList::iterator itEnd = _selectedJoints.end();
	PhysicsJointList::iterator nextIt = _selectedJoints.begin();
	PhysicsJointList::iterator it;
	while ( nextIt != itEnd)
	{
		it = nextIt;
		 ++nextIt;
		 validateJointAnchors(*it);		
	}
}

void PhysicsManager::rotateSelectedJoints( float angle, Vector2 rotationCenter )
{
	PhysicsJointList::iterator itEnd = _selectedJoints.end();
	for (PhysicsJointList::iterator it = _selectedJoints.begin(); it != itEnd; ++it)
	{
		(*it)->rotateAroundPoint(angle, rotationCenter);
	}
}





} // namespace PhySketch