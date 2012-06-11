
#include "PhySketchPhysicsManager.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"

namespace PhySketch
{

template<> PhysicsManager* Singleton<PhysicsManager>::ms_Singleton = 0;

PhysicsManager::PhysicsManager(Vector2 gravity)
{
	_renderer = Renderer::getSingletonPtr();
	_physicsWorld = new b2World(b2Vec2((float32)gravity.x, (float32)gravity.y));
	_simulationPaused = false;
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
	ASSERT(ms_Singleton != NULL);
	return ms_Singleton;
}

PhysicsManager& PhysicsManager::getSingleton( void )
{
	ASSERT(ms_Singleton != NULL);
	return *ms_Singleton;
}

void PhysicsManager::AddBody( PhysicsBody *b )
{
	ASSERT(b != nullptr && "PhysicsBody is NULL");
	_physicsBodies.push_back(b);
}

void PhysicsManager::RemoveBody( PhysicsBody *b )
{
	_physicsBodies.remove(b);
	int polygonToRemoveCount = b->_oldPolygons.size();
	for(int i = 0; i < polygonToRemoveCount; i++)
	{
		_renderer->removePolygon(b->_oldPolygons[i]);
	}
}

void PhysicsManager::Update( ulong advanceTime )
{
	PhysicsBody *pb = nullptr;
	Polygon *poly = nullptr;
	int polygonCount = 0;
	int polygonToRemoveCount = 0;
	int i = 0;

	if(!_simulationPaused)
	{
		stepPhysics(advanceTime);
	}

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
				_renderer->removePolygon(poly);
				delete poly;
				poly = nullptr;
			}

			polygonCount = pb->_polygons.size();
			for(i = 0; i < polygonCount; i++)
			{
				_renderer->addPolygon(pb->_polygons[i]);
			}
		}
		pb->update();
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

} // namespace PhySketch