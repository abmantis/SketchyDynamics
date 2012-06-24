
#include "PhySketchPhysicsManager.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"
#include "PhySketchPhysicsJoint.h"

namespace PhySketch
{

template<> PhysicsManager* Singleton<PhysicsManager>::ms_Singleton = 0;

PhysicsManager::PhysicsManager(Vector2 gravity) :
	_physicsBodiesIDSeed(0),
	_simulationPaused	(false)
{
	_renderer = Renderer::getSingletonPtr();
	_physicsWorld = new b2World(b2Vec2((float32)gravity.x, (float32)gravity.y));
}

PhysicsManager::~PhysicsManager()
{
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

void PhysicsManager::AddBody( PhysicsBody *b )
{
	PHYSKETCH_ASSERT(b != nullptr && "PhysicsBody is NULL");

	b->_id = ++_physicsBodiesIDSeed;
	_physicsBodies.push_back(b);

	int polygonCount = b->_polygons.size();
	for(int i = 0; i < polygonCount; i++)
	{
		_renderer->addPolygon(b->_polygons[i], b->_id);
	}
	b->_needsPolygonUpdate = false;
}

void PhysicsManager::RemoveBody( PhysicsBody *b )
{
	_physicsBodies.remove(b);
	int polygonToRemoveCount = b->_oldPolygons.size();
	for(int i = 0; i < polygonToRemoveCount; i++)
	{
		_renderer->removePolygon(b->_oldPolygons[i]);
	}
	polygonToRemoveCount = b->_polygons.size();
	for(int i = 0; i < polygonToRemoveCount; i++)
	{
		_renderer->removePolygon(b->_polygons[i]);
	}
}

void PhysicsManager::AddJoint( PhysicsJoint *j )
{
	PHYSKETCH_ASSERT(j != nullptr && "PhysicsJoint is NULL");

	j->_id = (static_cast<PhysicsBody*>(j->_joint->GetBodyA()->GetUserData()))->_id;
	_physicsJoints.push_back(j);
	_renderer->addPolygon(j->_polygon, j->_id, RQT_Scene);
}

void PhysicsManager::RemoveJoint( PhysicsJoint *j )
{
	_physicsJoints.remove(j);
	_renderer->removePolygon(j->_polygon);
}

void PhysicsManager::Update( ulong advanceTime )
{	

	if(!_simulationPaused)
	{
		stepPhysics(advanceTime);
	}

	//////////////////////////////////////////////////////////////////////////
	// Update bodies
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

				polygonCount = pb->_polygons.size();
				for(i = 0; i < polygonCount; i++)
				{
					_renderer->addPolygon(pb->_polygons[i], pb->_id, RQT_Scene);
				}
			}
			pb->update();
		}	
	}

	//////////////////////////////////////////////////////////////////////////
	// Update joints
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

void PhysicsManager::SelectBody( PhysicsBody *b )
{
	if(b->_selectable == true && b->_selected == false)
	{
		b->_selected = true;
		b->_polygons[1]->SetMaterial(b->_selectedMaterial);
		_selectedBodies.push_back(b);
	}
}

void PhysicsManager::UnselectBody( PhysicsBody *b )
{
	if(b->_selected)
	{
		b->_selected = false;
		b->_polygons[1]->SetMaterial(b->_lineMaterial);

		_selectedBodies.remove(b);
	}
}

void PhysicsManager::SetUnselectableBody( PhysicsBody *b )
{
	b->_selectable = false;
	UnselectBody(b);
}

void PhysicsManager::SetSelectableBody( PhysicsBody *b )
{
	b->_selectable = true;
}

void PhysicsManager::UnselectAllBodies()
{
	while(!_selectedBodies.empty())
	{
		UnselectBody(_selectedBodies.front());
	}
	
}

const PhysicsManager::PhysicsBodyList& PhysicsManager::getSelectedBodies() const
{
	return _selectedBodies;
}


} // namespace PhySketch