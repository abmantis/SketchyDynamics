#include "PhySketchPhysicsJoint.h"
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <Box2D/Dynamics/b2Body.h>
#include "PhySketchUtils.h"

namespace PhySketch
{
PhysicsJoint::PhysicsJoint( b2Joint *joint, PhysicsJointRepresentation representation, const Material& material, ulong id ) :
	Polygon(VV_Static, "PS_Joint" + toString(ulong(id))),
	_joint(joint), 
	_pjr(representation), 
	_material(material), 
	_id(id)
{
	_joint->SetUserData(this);

	//////////////////////////////////////////////////////////////////////////
	// Construct subpolygons
	Vector2 positionA(_joint->GetAnchorA());
	Vector2 positionB(_joint->GetAnchorB());

	switch (_pjr)
	{
	case PJR_Circle:			
		Polygon::CreateCircleSubPolygon(DM_LINE_LOOP, Vector2::ZERO_XY, 0.10f, 80);
		Polygon::setPosition(positionA);
		break;
	case PJR_Cross:
	{
		SubPolygon *subpoly = createSubPolygon(DM_LINES);
		subpoly->addVertex(Vector2(-0.10f,-0.10f));
		subpoly->addVertex(Vector2( 0.10f, 0.10f));
		subpoly->addVertex(Vector2(-0.10f, 0.10f));
		subpoly->addVertex(Vector2( 0.10f,-0.10f));
		Polygon::setPosition(positionA);
		Polygon::setAngle(_joint->GetBodyA()->GetAngle());
		break;
	}
	case PJR_Zigzag:
		break;
	case PJR_Line:
		break;
	}		

	_subPolygons[0]->SetMaterial(_material);
}

PhysicsJoint::~PhysicsJoint()
{
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
		Polygon::setPosition(Vector2(_joint->GetAnchorA()));
		break;
	case PJR_Cross:
		Polygon::setPosition(Vector2(_joint->GetAnchorA()));
		Polygon::setAngle(_joint->GetBodyA()->GetAngle());
		break;
	case PJR_Zigzag:
		break;
	case PJR_Line:
		break;
	}		
}

void PhysicsJoint::setAngle( float angle )
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicsJoint::setPosition( const Vector2& position )
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicsJoint::setScale( const Vector2& scale )
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicsJoint::translate( const Vector2& amount )
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicsJoint::rotate( const float& angle )
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicsJoint::scale( const Vector2& factor )
{
	throw std::exception("The method or operation is not implemented.");
}

bool PhysicsJoint::isPointInside( const Vector2& pt ) const
{
	return getWorldAABB(true).isPointInside(pt);
}

}

