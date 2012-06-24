#include "PhySketchPhysicsJoint.h"
#include "PhySketchPolygon.h"
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include "Box2D/Dynamics/b2Body.h"

namespace PhySketch
{
	PhysicsJoint::PhysicsJoint( b2Joint *joint, PhysicsJointRepresentation representation, const Material& material ) :
		_joint(joint), _pjr(representation), _material(material), _id(0)
	{
		_joint->SetUserData(this);

		//////////////////////////////////////////////////////////////////////////
		// Construct polygons
		Vector2 positionA(_joint->GetAnchorA());
		Vector2 positionB(_joint->GetAnchorB());
		_polygon = nullptr;

		switch (_pjr)
		{
		case PJR_Circle:			
			_polygon = Polygon::CreateCircle(Polygon::DM_LINE_LOOP, Vector2::ZERO, 0.10f, 180);
			_polygon->setPosition(positionA);
			break;
		case PJR_Cross:
			_polygon = new Polygon(Polygon::VV_Static, Polygon::DM_LINES);
			_polygon->addVertex(Vector2(-0.10f,-0.10f));
			_polygon->addVertex(Vector2( 0.10f, 0.10f));
			_polygon->addVertex(Vector2(-0.10f, 0.10f));
			_polygon->addVertex(Vector2( 0.10f,-0.10f));
			_polygon->setPosition(positionA);
			_polygon->setAngle(_joint->GetBodyA()->GetAngle());
			break;
		case PJR_Zigzag:
			break;
		case PJR_Line:
			break;
		}		

		_polygon->SetMaterial(_material);
	}
		
	const Material& PhysicsJoint::getMaterial() const
	{
		return(_material);
	}

	const b2Joint* PhysicsJoint::getBox2DJoint() const
	{
		return _joint;
	}

	void PhysicsJoint::update()
	{
		switch (_pjr)
		{
		case PJR_Circle:			
			_polygon->setPosition(Vector2(_joint->GetAnchorA()));
			break;
		case PJR_Cross:
			_polygon->setPosition(Vector2(_joint->GetAnchorA()));
			_polygon->setAngle(_joint->GetBodyA()->GetAngle());
			break;
		case PJR_Zigzag:
			break;
		case PJR_Line:
			break;
		}		
	}

}

