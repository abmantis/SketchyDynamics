#include "PhySketchPhysicsJoint.h"
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include "PhySketchUtils.h"

namespace PhySketch
{
PhysicsJoint::PhysicsJoint( b2Joint *joint, PhysicsJointType type, const Material& material, const Material& selectedMaterial, ulong id ) :
	Polygon				(VV_Static, "PS_Joint" + toString(ulong(id))),
	_joint				(joint), 
	_pjt				(type), 
	_material			(material), 
	_selectedMaterial	(selectedMaterial),
	_id					(id),
	_selectable			(true),
	_selected			(false)
{
	_joint->SetUserData(this);
}

PhysicsJoint::~PhysicsJoint()
{
}

const b2Joint* PhysicsJoint::getBox2DJoint() const
{
	return _joint;
}

void PhysicsJoint::setAngle( float angle )
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicsJoint::setPosition( const Vector2& position )
{
	PHYSKETCH_ASSERT(_selected && "Cannot setPosition on an unselected joint");
	Polygon::setPosition(position);
}

void PhysicsJoint::setScale( const Vector2& scale )
{
	throw std::exception("The method or operation is not implemented.");
}

void PhysicsJoint::translate( const Vector2& amount )
{
	PHYSKETCH_ASSERT(_selected && "Cannot translate an unselected joint");
	Polygon::translate(amount);
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
	return getTransformedAABB(true).isPointInside(pt);
}

void PhysicsJoint::select()
{
	_selected = true;
	_subPolygons[0]->SetMaterial(_selectedMaterial);
}

void PhysicsJoint::unselect()
{
	_selected = false;
	_subPolygons[0]->SetMaterial(_material);
}

bool PhysicsJoint::isSelectable() const
{
	return _selectable;
}

bool PhysicsJoint::isSelected() const
{
	return _selected;
}

//////////////////////////////////////////////////////////////////////////
// PhysicsJointRevolute class
PhysicsJointRevolute::PhysicsJointRevolute( b2RevoluteJoint *joint, const Material& material, const Material& selectedMaterial, ulong id ) :
	PhysicsJoint(joint, PJT_Revolute, material, selectedMaterial, id)
{
	Polygon::CreateCircleSubPolygon(DM_LINE_LOOP, Vector2::ZERO_XY, 0.10f, 80)->SetMaterial(_material);;
	Polygon::setPosition(_joint->GetAnchorA());
}

const b2RevoluteJoint* PhysicsJointRevolute::getBox2DRevoluteJoint() const
{
	return static_cast<b2RevoluteJoint*>(_joint);
}

void PhysicsJointRevolute::update()
{
	if(_selected)
	{
		// Do not sync the polygon with the b2d joint when it is selected
		return;
	}
	
	Polygon::setPosition(Vector2(_joint->GetAnchorA()));
}


PhysicsJointWeld::PhysicsJointWeld( b2WeldJoint *joint, const Material& material, const Material& selectedMaterial, ulong id ) :
	PhysicsJoint(joint, PJT_Weld, material, selectedMaterial, id)
{
	SubPolygon *subpoly = createSubPolygon(DM_LINES);
	subpoly->addVertex(Vector2(-0.10f,-0.10f));
	subpoly->addVertex(Vector2( 0.10f, 0.10f));
	subpoly->addVertex(Vector2(-0.10f, 0.10f));
	subpoly->addVertex(Vector2( 0.10f,-0.10f));
	subpoly->SetMaterial(_material);
	Polygon::setPosition(_joint->GetAnchorA());
	Polygon::setAngle(_joint->GetBodyA()->GetAngle());		
}

const b2WeldJoint* PhysicsJointWeld::getBox2DWeldJoint() const
{
	return static_cast<b2WeldJoint*>(_joint);
}

void PhysicsJointWeld::update()
{
	if(_selected)
	{
		// Do not sync the polygon with the b2d joint when it is selected
		return;
	}

	Polygon::setPosition(Vector2(_joint->GetAnchorA()));
	Polygon::setAngle(_joint->GetBodyA()->GetAngle());
}

}

