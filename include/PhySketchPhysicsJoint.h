#ifndef PhySketchPhysicsJoint_h__
#define PhySketchPhysicsJoint_h__

#include "PhySketchDefinitions.h"
#include "PhySketchMaterial.h"

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

	class PhysicsJoint
	{	
		friend class PhysicsManager;
	public:
		PhysicsJoint(b2Joint *joint, PhysicsJointRepresentation representation, const Material& material = Material(Color(1.0f, 0.3f, 0.3f, 0.0f)));
		virtual ~PhysicsJoint() {};
		
		virtual const b2Joint* getBox2DJoint() const;

		// Access the material
		virtual const Material& getMaterial() const;


		/// <summary> Updates . </summary>
		/// <remarks> This is normally not called by the "user". It is called automatically by PhySketch. </remarks>
		virtual void update();

	protected:
		PhysicsJointRepresentation _pjr;
		Polygon* _polygon;
		b2Joint* _joint;

		ulong _id;

		Material _material;
	};
}
#endif // PhySketchPhysicsJoint_h__