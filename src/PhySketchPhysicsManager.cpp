
#include "PhySketchPhysicsManager.h"
#include "PhySketchPhysicsEventsListener.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPhysicsJoint.h"
#include "PhySketchPolygon.h"
#include "PhySketchMaterialManager.h"


namespace PhySketch
{

template<> PhysicsManager* Singleton<PhysicsManager>::ms_Singleton = 0;

PhysicsManager::PhysicsManager(Vector2 gravity, Vector2 worldsize) :
	_physicsBodiesIDSeed				(0),
	_physicsJointsIDSeed				(0),
	_simulationPaused_user				(false),
	_simulationPaused_physketch			(false),
	_simulationPaused_selectedObjects	(false),
	_simulationPaused_prev				(false),
	_simulationPaused_internal_prev		(false),
	_worldSize							(worldsize)
{
	_renderer = Renderer::getSingletonPtr();

	//////////////////////////////////////////////////////////////////////////
	// Create default Materials
	MaterialManager* matMgr = MaterialManager::getSingletonPtr();	
	_defaultBodyFillMat		= matMgr->createMaterial("PS_defaultBodyFillMat",		Color(0.7f, 0.7f, 0.8f, 1.0f));
	_defaultBodyLineMat		= matMgr->createMaterial("PS_defaultBodyLineMat",		Color(0.3f, 0.3f, 1.0f, 1.0f));
	_defaultBodySelectedMat	= matMgr->createMaterial("PS_defaultBodySelectedMat",	Color(0.2f, 1.0f, 1.0f, 1.0f));
	_defaultJointMat		= matMgr->createMaterial("PS_defaultJointMat",			Color(1.0f, 0.0f, 0.0f, 0.8f));
	_defaultJointSelectedMat= matMgr->createMaterial("PS_defaultJointSelectedMat",	Color(1.0f, 0.0f, 1.0f, 1.0f));

	//////////////////////////////////////////////////////////////////////////
	// Init physics world 
	_physicsWorld = new b2World(b2Vec2((float32)gravity.x, (float32)gravity.y));
	_physicsWorld->SetDestructionListener(this);
	_physicsWorld->SetContactListener(this);

	//////////////////////////////////////////////////////////////////////////
	// create world bounds sensor to destroy bodies that go out of the worldsize
	b2BodyDef bodyDef;
	bodyDef.position.Set(0.0f, 0.0f);
	b2Body *worldBoundsSensorBody = _physicsWorld->CreateBody(&bodyDef);	
	b2Vec2 vs[4];
	vs[0].Set(-worldsize.x*0.5f,-worldsize.y*0.5f);
	vs[1].Set( worldsize.x*0.5f,-worldsize.y*0.5f);
	vs[2].Set( worldsize.x*0.5f, worldsize.y*0.5f);
	vs[3].Set(-worldsize.x*0.5f, worldsize.y*0.5f);
	b2ChainShape worldBoundsSensorBoxChain;
	worldBoundsSensorBoxChain.CreateLoop(vs, 4);
	b2FixtureDef fd;
	fd.shape = &worldBoundsSensorBoxChain;
	fd.isSensor = true;
	_worldBoundsSensor = worldBoundsSensorBody->CreateFixture(&fd);	

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

void PhysicsManager::SayGoodbye( b2Joint* joint )
{
	void *userdata = joint->GetUserData();
	if(userdata != nullptr)
	{
		PhysicsJoint *pj = static_cast<PhysicsJoint*>(userdata);
		destroyJoint(pj, false);
	}
}

void PhysicsManager::SayGoodbye( b2Fixture* fixture )
{
}

void PhysicsManager::BeginContact( b2Contact* contact )
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if (fixtureA == _worldBoundsSensor)
	{
		_bodiesToDestruct.insert(static_cast<PhysicsBody*>(fixtureB->GetBody()->GetUserData()));
	}
	else if (fixtureB == _worldBoundsSensor)
	{
		_bodiesToDestruct.insert(static_cast<PhysicsBody*>(fixtureA->GetBody()->GetUserData()));
	}
}

PhysicsBody* PhysicsManager::createBody( b2Body *b2d_body )
{
	// Create new PhysicsBody
	PhySketch::PhysicsBody *b = new PhySketch::PhysicsBody(b2d_body, 
		++_physicsBodiesIDSeed, _defaultBodyFillMat, _defaultBodyLineMat,
		_defaultBodySelectedMat);

	_physicsBodies.push_back(b);

	_renderer->addPolygon(b, b->_id);

	invokeListenersBodyCreated(b);

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
		_simulationPaused_selectedObjects = false;
	}
}

PhysicsJoint* PhysicsManager::createJoint( b2Joint *b2d_joint )
{
	PhysicsJoint *j = nullptr;
	b2JointType type = b2d_joint->GetType();
	
	// get the ID from the bodies
	ulong bodyA_id = (static_cast<PhysicsBody*>(b2d_joint->GetBodyA()->GetUserData()))->_id;
	ulong bodyB_id = (static_cast<PhysicsBody*>(b2d_joint->GetBodyB()->GetUserData()))->_id;

	switch (type)
	{
	case e_revoluteJoint:
		{
			b2RevoluteJoint* revj = static_cast<b2RevoluteJoint*>(b2d_joint);
			PhysicsJointRevolute* pjr = new PhysicsJointRevolute(revj, _defaultJointMat, _defaultJointSelectedMat, ++_physicsJointsIDSeed);
			_renderer->addPolygon(pjr->_poly, (bodyA_id > bodyB_id)? bodyA_id : bodyB_id, RQT_Scene);
			j = pjr;
			break;
		}
	case e_weldJoint:
		{
			b2WeldJoint* weldj = static_cast<b2WeldJoint*>(b2d_joint);
			PhysicsJointWeld* pjw = new PhysicsJointWeld(weldj, _defaultJointMat, _defaultJointSelectedMat, ++_physicsJointsIDSeed);
			_renderer->addPolygon(pjw->_poly, (bodyA_id > bodyB_id)? bodyA_id : bodyB_id, RQT_Scene);
			j = pjw;
			break;
		}
	case e_distanceJoint:
		{
			b2DistanceJoint* distj = static_cast<b2DistanceJoint*>(b2d_joint);
			PhysicsJointDistance* pjd = new PhysicsJointDistance(distj, _defaultJointMat, _defaultJointSelectedMat, ++_physicsJointsIDSeed);
			_renderer->addPolygon(pjd->_zigZagPoly, (bodyA_id > bodyB_id)? bodyA_id : bodyB_id, RQT_Scene);
			_renderer->addPolygon(pjd->_circlePolyA, (bodyA_id > bodyB_id)? bodyA_id : bodyB_id, RQT_Scene);
			_renderer->addPolygon(pjd->_circlePolyB, (bodyA_id > bodyB_id)? bodyA_id : bodyB_id, RQT_Scene);
			j = pjd;
			break;
		}
	default:
		throw std::exception("Unsupported joint type");
		return NULL;
	}

	 
	_physicsJoints.push_back(j);
	invokeListenersJointCreated(j);

	return j;
}

void PhysicsManager::destroyJoint( PhysicsJoint* joint, bool destroyB2DJoint /*= true*/ )
{
	_physicsJoints.remove(joint);
	_selectedJoints.remove(joint);

	switch(joint->_pjt)
	{	
	case PJT_Weld:
		{
			PhysicsJointWeld *pjw = dynamic_cast<PhysicsJointWeld*>(joint);
			_renderer->removePolygon(pjw->_poly);
		}
		break;
	case PJT_Revolute:
		{
			PhysicsJointRevolute *pjr = dynamic_cast<PhysicsJointRevolute*>(joint);
			_renderer->removePolygon(pjr->_poly);
		}
		break;
	case PJT_Distance:
		{
			PhysicsJointDistance *pjd = dynamic_cast<PhysicsJointDistance*>(joint);
			_renderer->removePolygon(pjd->_zigZagPoly);
			_renderer->removePolygon(pjd->_circlePolyA);
			_renderer->removePolygon(pjd->_circlePolyB);
		}
		break;
// 	case PJT_Rope:
// 		break;	
	default:
		throw std::exception("Unknown PhysicsJointType");
		break;
	}

	if(destroyB2DJoint)
	{
		_physicsWorld->DestroyJoint(joint->_joint);
	}

	delete joint;
	joint = nullptr;

	if(_selectedBodies.size() == 0 && _selectedJoints.size() == 0)
	{
		_simulationPaused_selectedObjects = false;
	}
}

void PhysicsManager::update( ulong advanceTime )
{	
	bool simPausedInternal = (_simulationPaused_selectedObjects || _simulationPaused_physketch);

	if(!_simulationPaused_user && !simPausedInternal)
	{
		stepPhysics(advanceTime);
	}

	{
		// Destroy bodies
		PhysicsBodySet::iterator it = _bodiesToDestruct.begin();
		PhysicsBodySet::iterator itEnd = _bodiesToDestruct.end();
		for( ; it != itEnd; ++it)
		{
			destroyBody(*it);
		}
		_bodiesToDestruct.clear();
	}
	
	{
		// Update joints
		PhysicsJointList::iterator itEnd = _physicsJoints.end();
		for (PhysicsJointList::iterator it = _physicsJoints.begin(); it != itEnd; ++it)
		{
			(*it)->update(advanceTime);
		}
	}

	if(_simulationPaused_user != _simulationPaused_prev)
	{
		_simulationPaused_prev = _simulationPaused_user;
		invokeListenersSimulationStateChanged(_simulationPaused_user);
	}
	
	if( simPausedInternal != _simulationPaused_internal_prev)
	{
		_simulationPaused_internal_prev = simPausedInternal;
		invokeListenersSimulationInternalStateChanged(simPausedInternal);
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
	_simulationPaused_user = true;
}

void PhysicsManager::playSimulation()
{
	_simulationPaused_user = false;
}

void PhysicsManager::toggleSimulation()
{
	_simulationPaused_user = !_simulationPaused_user;
}

bool PhysicsManager::isSimulationPaused() const
{
	return _simulationPaused_user || _simulationPaused_physketch || _simulationPaused_selectedObjects;
}

void PhysicsManager::selectBody( PhysicsBody *b )
{
	if(b->_selectable == true && b->_selected == false)
	{
		uint subPolyCount = b->getSubPolygonCount();
		for (uint i = 1; i < subPolyCount; i += 2)
		{
			b->getSubPolygon(i)->setMaterial(b->_selectedMaterial);
		}

		b->_selected = true;
		_selectedBodies.push_back(b);
		
		// put the body in sleep so that forces stop being applied
		b->_body->SetAwake(false);
		selectConnectedBodiesRecurse(b);

		_simulationPaused_selectedObjects = true;
	}
}

void PhysicsManager::unselectBody( PhysicsBody *b )
{
	if(b->_selected)
	{
		uint subPolyCount = b->getSubPolygonCount();
		for (uint i = 1; i < subPolyCount; i += 2)
		{
			b->getSubPolygon(i)->setMaterial(b->_lineMaterial);
		}

		b->_selected = false;
		_selectedBodies.remove(b);

		// wake up the body
		b->_body->SetAwake(true);
		unselectConnectedBodiesRecurse(b);
		
		if(_selectedBodies.size() == 0 && _selectedJoints.size() == 0)
		{
			_simulationPaused_selectedObjects = false;
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

void PhysicsManager::destroySelectedBodies()
{
	PhysicsBodyList::iterator itEnd = _selectedBodies.end();
	PhysicsBodyList::iterator nextIt = _selectedBodies.begin();
	PhysicsBodyList::iterator it;
	while ( nextIt != itEnd)
	{
		it = nextIt;
		++nextIt;
		destroyBody(*it, true);
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

void PhysicsManager::scaleSelectedBodies( Vector2 factor, Vector2 scaleCenter )
{
	PhysicsBodyList::iterator itEnd = _selectedBodies.end();
	for (PhysicsBodyList::iterator it = _selectedBodies.begin(); it != itEnd; ++it)
	{
		(*it)->scaleAroundPoint(factor, scaleCenter);
	}

	// translate joints so they match the new body scaled position
	PhysicsJoint *pj = nullptr;
	PhysicsJointList::iterator jointItEnd = _selectedJoints.end();
	for (PhysicsJointList::iterator jointIt = _selectedJoints.begin(); jointIt != jointItEnd; ++jointIt)
	{
		pj = *jointIt;
		switch(pj->_pjt)
		{	
		case PJT_Weld:
			{
				PhysicsJointWeld *pjw = dynamic_cast<PhysicsJointWeld*>(pj);
				Vector2 pos = pjw->getPosition();
				pos = scaleCenter + (pos - scaleCenter) * factor;
				pjw->setPosition(pos);
			}
			break;
		case PJT_Revolute:
			{
				PhysicsJointRevolute *pjr = dynamic_cast<PhysicsJointRevolute*>(pj);
				Vector2 pos = pjr->getPosition();
				pos = scaleCenter + (pos - scaleCenter) * factor;
				pjr->setPosition(pos);
			}
			break;
		case PJT_Distance:
			{
				// TODO: when one of the bodies is not selectable, the respective anchor should not move
				PhysicsJointDistance *pjd = dynamic_cast<PhysicsJointDistance*>(pj);
				Vector2 posA = pjd->getPositionA();
				Vector2 posB = pjd->getPositionB();
				posA = scaleCenter + (posA - scaleCenter) * factor;
				posB = scaleCenter + (posB - scaleCenter) * factor;
				pjd->setPositions(posA, posB);
			}
			break;
			// 	case PJT_Rope:
			// 		break;	
		default:
			throw std::exception("Unknown PhysicsJointType");
			break;
		}
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
		_simulationPaused_selectedObjects = true;
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
			_simulationPaused_selectedObjects = false;
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
		PhysicsJoint* j = (*it);
		j->translate(translation);
		JointAnchorsSituation jas = j->checkAnchorsSituation(true);		
	}
}

bool PhysicsManager::validateJointAnchors( PhysicsJoint *j )
{
	JointAnchorsSituation jas = j->checkAnchorsSituation(false);
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
					PhysicsJointRevolute* pjr = dynamic_cast<PhysicsJointRevolute*>(j);
					jd.Initialize(revj->GetBodyA(), revj->GetBodyB(), pjr->getPosition().tob2Vec2());
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
					PhysicsJointWeld* pjw = dynamic_cast<PhysicsJointWeld*>(j);
					jd.Initialize(weldj->GetBodyA(), weldj->GetBodyB(), pjw->getPosition().tob2Vec2());
					jd.collideConnected	= weldj->GetCollideConnected();
					jd.dampingRatio		= weldj->GetDampingRatio();
					jd.frequencyHz		= weldj->GetFrequency();
					newb2djoint			= _physicsWorld->CreateJoint(&jd);
					break;
				}
				case e_distanceJoint:
				{
					b2DistanceJointDef jd;
					b2DistanceJoint* distj = static_cast<b2DistanceJoint*>(oldb2djoint);
					PhysicsJointDistance* pjd = dynamic_cast<PhysicsJointDistance*>(j);
					jd.Initialize(distj->GetBodyA(), distj->GetBodyB(), pjd->getPositionA().tob2Vec2(), pjd->getPositionB().tob2Vec2());
					jd.collideConnected	= distj->GetCollideConnected();
					jd.dampingRatio		= distj->GetDampingRatio();
					jd.frequencyHz		= distj->GetFrequency();
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

void PhysicsManager::destroySelectedJoints()
{
	PhysicsJointList::iterator itEnd = _selectedJoints.end();
	PhysicsJointList::iterator nextIt = _selectedJoints.begin();
	PhysicsJointList::iterator it;
	while ( nextIt != itEnd)
	{
		it = nextIt;
		++nextIt;
		destroyJoint(*it, true);
	}
}

void PhysicsManager::addEventListener( PhysicsEventsListener *listener )
{
	_eventListeners.insert(listener);
}

void PhysicsManager::removeEventListener( PhysicsEventsListener *listener )
{
	_eventListeners.erase(listener);
}

void PhysicsManager::invokeListenersSimulationStateChanged( bool paused )
{
	for(std::set<PhysicsEventsListener*>::iterator iter = _eventListeners.begin(); 
		iter != _eventListeners.end(); iter++)
	{
		(*iter)->simulationStateChanged(paused);
	}
}

void PhysicsManager::invokeListenersSimulationInternalStateChanged( bool paused )
{
	for(std::set<PhysicsEventsListener*>::iterator iter = _eventListeners.begin(); 
		iter != _eventListeners.end(); iter++)
	{
		(*iter)->simulationInternalStateChanged(paused);
	}
}

void PhysicsManager::invokeListenersBodyCreated( PhysicsBody *body )
{
	for(std::set<PhysicsEventsListener*>::iterator iter = _eventListeners.begin(); 
		iter != _eventListeners.end(); iter++)
	{
		(*iter)->bodyCreated(body);
	}
}

void PhysicsManager::invokeListenersJointCreated( PhysicsJoint *joint )
{
	for(std::set<PhysicsEventsListener*>::iterator iter = _eventListeners.begin(); 
		iter != _eventListeners.end(); iter++)
	{
		(*iter)->jointCreated(joint);
	}
}


} // namespace PhySketch