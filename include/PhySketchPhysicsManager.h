#ifndef PhySketchPhysicsBodyManager_h__
#define PhySketchPhysicsBodyManager_h__

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"
#include "Box2D/Box2D.h"
#include "PhySketchPhysicsJoint.h"


namespace PhySketch
{
	class PhysicsManager : public Singleton<PhysicsManager>, public b2DestructionListener, private b2ContactListener
	{	
	public:
		typedef std::list<PhysicsBody*> PhysicsBodyList;
		typedef std::set<PhysicsBody*> PhysicsBodySet;
		typedef std::list<PhysicsJoint*> PhysicsJointList;

	public:
		PhysicsManager(Vector2 gravity, Vector2 worldsize);
		virtual ~PhysicsManager();

		/// <summary> Creates a new PhysicsBody. </summary>
		/// <param name="b2d_body"> The Box2D body to be attached to this
		/// 	PhysicsBody. </param>
		/// <returns> The new PhysicsBody. </returns>
		virtual PhysicsBody* createBody(b2Body *b2d_body);

		/// <summary> Creates a new PhysicsBody and an attached b2Body based on a
		/// 	b2BodyDef. </summary>
		/// <remarks> In addition to creating a new PhysicsBody this method also
		/// 	creates a new b2Body. DO NOT FORGET to call
		/// 	PhysicsBody::reconstructPolygons() after adding fixtures to the
		/// 	body. </remarks>
		/// <param name="b2d_body_def"> The definition of the new Box2D body to
		/// 	be attached to this PhysicsBody. </param>
		/// <returns> The new PhysicsBody. </returns>
		virtual PhysicsBody* createBody(const b2BodyDef& b2d_body_def);

		/// <summary> Destroys a PhysicsBody. </summary>
		/// <param name="b"> The PhysicsBody to add. </param>
		/// <param name="destroyB2DBody"> True to also destroy the Box2D b2Body
		/// 	attached to this object. </param>
		virtual void destroyBody(PhysicsBody *b, bool destroyB2DBody = true);

		virtual void selectBody(PhysicsBody *b);
		virtual void unselectBody(PhysicsBody *b);
		virtual void setUnselectableBody(PhysicsBody *b);
		virtual void setSelectableBody(PhysicsBody *b);
		virtual void unselectAllBodies();
		virtual void destroySelectedBodies();
		virtual void translateSelectedBodies(Vector2 translation);
		virtual void rotateSelectedBodies(float angle, Vector2 rotationCenter);
		virtual void scaleSelectedBodies(Vector2 factor, Vector2 scaleCenter);
		virtual void setActiveOnSelectedBodies(bool flag);
		virtual void setAwakeOnSelectedBodies(bool flag);
		virtual AABB getSelectedBodiesAABB() const;
		virtual const PhysicsBodyList& getSelectedBodies() const;

		/// <summary> Creates a new PhysicsJoint. </summary>
		/// <param name="b2d_joint"> The box2d joint. </param>		
		/// <returns> The new PhysicsJoint. </returns>
		virtual PhysicsJoint* createJoint( b2Joint *b2d_joint);

		/// <summary> Destroys a PhysicsJoint. </summary>
		/// <param name="joint"> The PhysicsJoint to be destroyed. </param>
		virtual void destroyJoint(PhysicsJoint* joint, bool destroyB2DJoint = true);

		virtual void selectJoint(PhysicsJoint *j);
		virtual void unselectJoint(PhysicsJoint *j);
		virtual void setUnselectableJoint(PhysicsJoint *j);
		virtual void setSelectableJoint(PhysicsJoint *j);
		virtual void unselectAllJoints();
		virtual const PhysicsJointList& getSelectedJoints() const;
		virtual void destroySelectedJoints();
		virtual void translateSelectedJoints(Vector2 translation);
		virtual void rotateSelectedJoints(float angle, Vector2 rotationCenter);

		/// <summary> Check if a joint was moved and if its anchor points are
		/// 	still inside the respective bodies. The joint is recreated if it
		/// 	was moved and the anchor points remain inside the bodies, or
		/// 	destroyed if the anchors are outside one of the bodies. </summary>
		/// <remarks> Deadvirus, 7/6/2012. </remarks>
		/// <param name="joint"> The PhysicsJoint to be validated. </param>
		/// <returns> false if the joint is invalid and so was destroyed, true if
		/// 	it is still valid. </returns>
		virtual bool validateJointAnchors(PhysicsJoint *j);
		virtual void validateSelectedJointsAnchors();


		// virtual void translateSelectedJoints(Vector2 translation);

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

		// Access the DefaultBodyFillMat
		Material* getDefaultBodyFillMat(void) 								{ return(_defaultBodyFillMat);				}
		void setDefaultBodyFillMat(Material* defaultBodyFillMat)			{ _defaultBodyFillMat = defaultBodyFillMat;	}
		// Access the DefaultBodyLineMat
		Material* getDefaultBodyLineMat(void) 								{ return(_defaultBodyLineMat);				}
		void setDefaultBodyLineMat(Material* defaultBodyLineMat)			{ _defaultBodyLineMat = defaultBodyLineMat;	}
		// Access the DefaultBodySelectedMat
		Material* getDefaultBodySelectedMat(void) 							{ return(_defaultBodySelectedMat);					}
		void setDefaultBodySelectedMat(Material* defaultBodySelectedMat)	{ _defaultBodySelectedMat = defaultBodySelectedMat;	}
		// Access the DefaultJointMat
		Material* getDefaultJointMat(void) 									{ return(_defaultJointMat);				}
		void setDefaultJointMat(Material* defaultJointMat)					{ _defaultJointMat = defaultJointMat;	}
		// Access the DefaultJointSelectedMat
		Material* getDefaultJointSelectedMat(void) const					{ return(_defaultJointSelectedMat);						}
		void setDefaultJointSelectedMat(Material* defaultJointSelectedMat)	{ _defaultJointSelectedMat = defaultJointSelectedMat;	}
		

		static PhysicsManager* getSingletonPtr(void);
		static PhysicsManager& getSingleton(void);

	protected:
		virtual void stepPhysics(ulong ellapsedMillisec);

		virtual void selectConnectedBodiesRecurse(PhysicsBody *b);
		virtual void unselectConnectedBodiesRecurse(PhysicsBody *b);

		/// <summary> b2DestructionListener method. </summary>
		virtual void SayGoodbye(b2Joint* joint);
		/// <summary> b2DestructionListener method. </summary>
		virtual void SayGoodbye( b2Fixture* fixture );
		/// <summary> b2ContactListener method. </summary>
		virtual void BeginContact( b2Contact* contact );


		


	protected:
		PhysicsBodyList _physicsBodies;
		PhysicsBodyList _selectedBodies;
		PhysicsBodySet _bodiesToDestruct;
		PhysicsJointList _physicsJoints;
		PhysicsJointList _selectedJoints;
		ulong _physicsBodiesIDSeed;
		ulong _physicsJointsIDSeed;
		Renderer* _renderer;
		b2World *_physicsWorld;
		Vector2 _worldSize;
		b2Fixture* _worldBoundsSensor;
		bool _simulationPaused;
		bool _simulationPaused_internal;

		Material* _defaultBodyFillMat;
		Material* _defaultBodyLineMat;
		Material* _defaultBodySelectedMat;
		Material* _defaultJointMat;
		Material* _defaultJointSelectedMat;
		
	};
}
#endif // PhySketchPhysicsBodyManager_h__