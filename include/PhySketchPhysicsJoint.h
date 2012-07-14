#ifndef PhySketchPhysicsJoint_h__
#define PhySketchPhysicsJoint_h__

#include "PhySketchDefinitions.h"
#include "PhySketchMaterial.h"
#include "PhySketchPolygon.h"

class b2Joint;
class b2RevoluteJoint;
class b2WeldJoint;
class b2DistanceJoint;

namespace PhySketch
{
	enum PhysicsJointType
	{
		PJT_Revolute,
		PJT_Weld,
		PJT_Distance,
		PJT_Rope,
	};

	enum JointAnchorsSituation
	{
		JAS_NOT_MOVED,	// Joint was not manually moved
		JAS_MOVED,		// Joint was moved but remains inside both bodies
		JAS_MOVED_OUT	// Joint was moved and is now outside both bodies
	};

	class PhysicsJoint : public Polygon
	{	
	protected:
		friend class PhysicsManager;

		PhysicsJoint(b2Joint *joint, PhysicsJointType type, Material* material, Material* selectedMaterial, ulong id);
		virtual ~PhysicsJoint();
	
	public:				
		virtual b2Joint* getBox2DJoint();

		virtual void update(ulong timeSinceLastFrame) = 0;

		virtual void setAngle( float angle );

		virtual void setPosition( const Vector2& position );

		virtual void setScale( const Vector2& scale );

		virtual void translate( const Vector2& amount );

		virtual void rotate( const float& angle );

		virtual void rotateAroundPoint( float angle, const Vector2& rotationPoint );

		virtual void scale( const Vector2& factor );

		virtual bool isPointInside( const Vector2& pt ) const;

		virtual bool isSelectable() const;
		virtual bool isSelected() const;

		virtual JointAnchorsSituation checkAnchorsSituation() const = 0;
		
	protected:
		virtual void select();
		virtual void unselect();		

	protected:
		PhysicsJointType _pjt;
		b2Joint* _joint;

		ulong _id;

		Material* _material;
		Material* _selectedMaterial;

		bool _selected;
		bool _selectable;
	};

	class PhysicsJointRevolute : public PhysicsJoint
	{	
	protected:
		friend class PhysicsManager;

		PhysicsJointRevolute(b2RevoluteJoint *joint, Material* material, Material* selectedMaterial, ulong id);
		virtual ~PhysicsJointRevolute() {};

	public:				
		virtual b2RevoluteJoint* getBox2DRevoluteJoint();

		virtual void update(ulong timeSinceLastFrame);

		virtual JointAnchorsSituation checkAnchorsSituation() const;

	};

	class PhysicsJointWeld : public PhysicsJoint
	{	
	protected:
		friend class PhysicsManager;

		PhysicsJointWeld(b2WeldJoint *joint, Material* material, Material* selectedMaterial, ulong id);
		virtual ~PhysicsJointWeld() {};

	public:				
		virtual b2WeldJoint* getBox2DWeldJoint();

		virtual void update(ulong timeSinceLastFrame);

		virtual JointAnchorsSituation checkAnchorsSituation() const;

	};

	class PhysicsJointDistance : public PhysicsJoint
	{	
	protected:
		friend class PhysicsManager;

		PhysicsJointDistance(b2DistanceJoint *joint, Material* material, Material* selectedMaterial, ulong id);
		virtual ~PhysicsJointDistance() {};

	public:				
		virtual b2DistanceJoint* getBox2DDistanceJoint();

		virtual void update(ulong timeSinceLastFrame);

		virtual JointAnchorsSituation checkAnchorsSituation() const;

	};
}
#endif // PhySketchPhysicsJoint_h__