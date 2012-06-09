#ifndef PhySketchPhysicsBodyManager_h__
#define PhySketchPhysicsBodyManager_h__

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"

namespace PhySketch
{
	class PhysicsManager : public Singleton<PhysicsManager>
	{	
	public:
		PhysicsManager();
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
		virtual void Update(double advanceTime);

		static PhysicsManager* getSingletonPtr(void);
		static PhysicsManager& getSingleton(void);
		
	protected:
		typedef std::list<PhysicsBody*> PhysicsBodyList;

		PhysicsBodyList _physicsBodies;
		Renderer* _renderer;
		
	};
}
#endif // PhySketchPhysicsBodyManager_h__