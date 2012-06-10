#ifndef PhySketchPhysicsBodyManager_h__
#define PhySketchPhysicsBodyManager_h__

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"
#include "Box2D/Box2D.h"


namespace PhySketch
{
	class PhysicsManager : public Singleton<PhysicsManager>
	{	
	public:
		PhysicsManager(Vector2 gravity);
		virtual ~PhysicsManager();

		/// <summary> Adds a physics body. </summary>
		/// <remarks> This class saves this pointer! It should not be deleted
		/// 	without calling RemoveBody first. </remarks>
		/// <param name="b"> The PhysicsBody to add. </param>
		virtual void AddBody(PhysicsBody *b);

		/// <summary> Removes a body. </summary>
		/// <remarks> This only removes the body from the bodies list and doesn't
		/// 	destroys it. </remarks>
		/// <param name="b"> The PhysicsBody to remove. </param>
		virtual void RemoveBody(PhysicsBody *b);

		/// <summary> Updates the physics objects and advances in the simulation
		/// 	by 'advanceTime' millisecconds. </summary>
		/// <param name="advanceTime"> Millisecconds to advance in the
		/// 	simulation. </param>
		virtual void Update(ulong advanceTime);

		/// <summary> Gets the physics world. </summary>
		/// <returns> The physics world. </returns>
		virtual b2World* getPhysicsWorld() const;

		static PhysicsManager* getSingletonPtr(void);
		static PhysicsManager& getSingleton(void);

	protected:
		virtual void stepPhysics(ulong ellapsedMillisec);
		
	protected:
		typedef std::list<PhysicsBody*> PhysicsBodyList;

		PhysicsBodyList _physicsBodies;
		Renderer* _renderer;
		b2World *_physicsWorld;
		
	};
}
#endif // PhySketchPhysicsBodyManager_h__