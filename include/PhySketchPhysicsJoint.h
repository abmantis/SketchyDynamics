#ifndef PhySketchPhysicsJoint_h__
#define PhySketchPhysicsJoint_h__

#include "PhySketchDefinitions.h"
#include "PhySketchMaterial.h"
#include "PhySketchPolygon.h"

class b2Joint;
namespace PhySketch
{
	enum PhysicsJointRepresentation
	{
		PJR_Circle,
		PJR_Cross,
		PJR_Zigzag,
		PJR_Line,
	};

	class PhysicsJoint : public Polygon
	{	
	protected:
		friend class PhysicsManager;

		PhysicsJoint(b2Joint *joint, PhysicsJointRepresentation representation, const Material& material, ulong id);
		virtual ~PhysicsJoint();
	
	public:				
		virtual const b2Joint* getBox2DJoint() const;

		/// <summary> Updates . </summary>
		/// <remarks> This is normally not called by the "user". It is called automatically by PhySketch. </remarks>
		virtual void update();

		virtual void setAngle( float angle );

		virtual void setPosition( const Vector2& position );

		virtual void setScale( const Vector2& scale );

		virtual void translate( const Vector2& amount );

		virtual void rotate( const float& angle );

		virtual void scale( const Vector2& factor );

		virtual bool isPointInside( const Vector2& pt ) const;

	protected:
		PhysicsJointRepresentation _pjr;
		b2Joint* _joint;

		ulong _id;

		Material _material;
	};
}
#endif // PhySketchPhysicsJoint_h__