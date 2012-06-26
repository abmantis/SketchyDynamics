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
		typedef std::list<PhysicsBody*> PhysicsBodyList;
		typedef std::list<PhysicsJoint*> PhysicsJointList;

	public:
		PhysicsManager(Vector2 gravity);
		virtual ~PhysicsManager();

		/// <summary> Adds a physics body. </summary>
		/// <remarks> This class saves this pointer! It should not be deleted
		/// 	without calling removeBody first. </remarks>
		/// <param name="b"> The PhysicsBody to add. </param>
		virtual void addBody(PhysicsBody *b);

		/// <summary> Removes a body. </summary>
		/// <remarks> This only removes the body from the bodies list and doesn't
		/// 	destroys it. </remarks>
		/// <param name="b"> The PhysicsBody to remove. </param>
		virtual void removeBody(PhysicsBody *b);

		virtual void selectBody(PhysicsBody *b);
		virtual void unselectBody(PhysicsBody *b);
		virtual void setUnselectableBody(PhysicsBody *b);
		virtual void setSelectableBody(PhysicsBody *b);

		virtual void unselectAllBodies();

		virtual void translateSelectedBodies(Vector2 translation);

		virtual void setActiveOnSelectedBodies(bool flag);
		virtual void setAwakeOnSelectedBodies(bool flag);

		virtual AABB getSelectedBodiesAABB() const;

		virtual const PhysicsBodyList& getSelectedBodies() const;

		/// <summary> Adds a joint. </summary>
		/// <remarks> This class saves this pointer! It should not be deleted
		/// 	without calling removeBody first. </remarks>
		/// <param name="j"> The PhysicsJoint to add. </param>
		virtual void addJoint(PhysicsJoint *j);

		/// <summary> Removes a joint. </summary>
		/// <remarks> This only removes the joint from the bodies list and doesn't
		/// 	destroys it. </remarks>
		/// <param name="j"> The PhysicsJoint to remove. </param>
		virtual void removeJoint(PhysicsJoint *j);

		/// <summary> Updates the physics objects and advances in the simulation
		/// 	by 'advanceTime' millisecconds. </summary>
		/// <param name="advanceTime"> Millisecconds to advance in the
		/// 	simulation. </param>
		virtual void update(ulong advanceTime);

		/// <summary> Pause the physics simulation. </summary>		
		virtual void pauseSimulation();

		/// <summary> Play/resume the physics simulation. </summary>
		virtual void playSimulation();

		/// <summary> Pauses the simulation if it was playing or plays it if it
		/// 	was paused. </summary>
		/// <remarks> Deadvirus, 6/10/2012. </remarks>
		virtual void toggleSimulation();

		virtual bool isSimulationPaused() const;

		/// <summary> Gets the physics world. </summary>
		/// <returns> The physics world. </returns>
		virtual b2World* getPhysicsWorld() const;

		static PhysicsManager* getSingletonPtr(void);
		static PhysicsManager& getSingleton(void);

	protected:
		virtual void stepPhysics(ulong ellapsedMillisec);
		
	protected:
		PhysicsBodyList _physicsBodies;
		PhysicsBodyList _selectedBodies;
		PhysicsJointList _physicsJoints;
		ulong _physicsBodiesIDSeed;
		Renderer* _renderer;
		b2World *_physicsWorld;
		bool _simulationPaused;
		
	};
}
#endif // PhySketchPhysicsBodyManager_h__