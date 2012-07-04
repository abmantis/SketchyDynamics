#include "PhySketchPhysicsJoint.h"
#include "PhySketchPolygon.h"
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include "Box2D/Dynamics/b2Body.h"
#include "PhySketchUtils.h"

namespace PhySketch
{
	PhysicsJoint::PhysicsJoint( b2Joint *joint, PhysicsJointRepresentation representation, const Material& material, ulong id ) :
		_joint(joint), _pjr(representation), _material(material), _id(id)
	{
		_joint->SetUserData(this);

		std::string jointPolyName = "PS_Joint" + toString(_id);

		//////////////////////////////////////////////////////////////////////////
		// Construct polygons
		Vector2 positionA(_joint->GetAnchorA());
		Vector2 positionB(_joint->GetAnchorB());
		_polygon = nullptr;

		switch (_pjr)
		{
		case PJR_Circle:			
			_polygon = Polygon::CreateCircle(DM_LINE_LOOP, Vector2::ZERO_XY, 0.10f, 180, jointPolyName);
			_polygon->setPosition(positionA);
			break;
		case PJR_Cross:
		{
			_polygon = new Polygon(VV_Static, jointPolyName);
			SubPolygon *subpoly = _polygon->createSubPolygon(DM_LINES);
			subpoly->addVertex(Vector2(-0.10f,-0.10f));
			subpoly->addVertex(Vector2( 0.10f, 0.10f));
			subpoly->addVertex(Vector2(-0.10f, 0.10f));
			subpoly->addVertex(Vector2( 0.10f,-0.10f));
			_polygon->setPosition(positionA);
			_polygon->setAngle(_joint->GetBodyA()->GetAngle());
			break;
		}
		case PJR_Zigzag:
			break;
		case PJR_Line:
			break;
		}		

		_polygon->getSubPolygon(0)->SetMaterial(_material);
	}

	PhysicsJoint::~PhysicsJoint()
	{
		delete _polygon;
		_polygon = nullptr;
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

