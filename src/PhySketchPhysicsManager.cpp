
#include "PhySketchPhysicsManager.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"

namespace PhySketch
{

template<> PhysicsManager* Singleton<PhysicsManager>::ms_Singleton = 0;

PhysicsManager::PhysicsManager()
{
	_renderer = Renderer::getSingletonPtr();
}

PhysicsManager::~PhysicsManager()
{

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

void PhysicsManager::Update( double advanceTime )
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

	// TODO: update b2d world here
}

} // namespace PhySketch