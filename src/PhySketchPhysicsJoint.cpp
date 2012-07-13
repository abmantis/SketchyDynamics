#include "PhySketchPhysicsJoint.h"
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include "PhySketchUtils.h"

namespace PhySketch
{
PhysicsJoint::PhysicsJoint( b2Joint *joint, PhysicsJointType type, Material* material, Material* selectedMaterial, ulong id ) :
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

b2Joint* PhysicsJoint::getBox2DJoint()
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

void PhysicsJoint::rotateAroundPoint( float angle, const Vector2& rotationPoint )
{
	PHYSKETCH_ASSERT(_selected && "Cannot rotateAroundPoint an unselected joint");
	Polygon::rotateAroundPoint(angle, rotationPoint);
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
	_subPolygons[0]->setMaterial(_selectedMaterial);
}

void PhysicsJoint::unselect()
{
	_selected = false;
	_subPolygons[0]->setMaterial(_material);
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
PhysicsJointRevolute::PhysicsJointRevolute( b2RevoluteJoint *joint, Material* material, Material* selectedMaterial, ulong id ) :
	PhysicsJoint(joint, PJT_Revolute, material, selectedMaterial, id)
{
	Polygon::CreateCircleSubPolygon(DM_LINE_LOOP, Vector2::ZERO_XY, 0.10f, 80)->setMaterial(_material);;
	Polygon::setPosition(_joint->GetAnchorA());
}

b2RevoluteJoint* PhysicsJointRevolute::getBox2DRevoluteJoint()
{
	return static_cast<b2RevoluteJoint*>(_joint);
}

void PhysicsJointRevolute::update(ulong timeSinceLastFrame)
{
	if(_selected)
	{
		// Do not sync the polygon with the b2d joint when it is selected
		return;
	}
	
	Polygon::setPosition(Vector2(_joint->GetAnchorA()));
}

PhySketch::JointAnchorsSituation PhysicsJointRevolute::checkAnchorsSituation() const
{
	// TODO: improve anchor check (sometimes A and B anchors can be different)
	if(Vector2(_joint->GetAnchorA()) != _position || Vector2(_joint->GetAnchorB()) != _position)
	{
		// The joint polygon was manually moved. Check if the joint is still inside both bodies
		Polygon* bA = static_cast<Polygon*>(_joint->GetBodyA()->GetUserData());
		Polygon* bB = static_cast<Polygon*>(_joint->GetBodyB()->GetUserData());		
		if(bA->isPointInside(_position) && bB->isPointInside(_position))
		{
			// the joint was only moved INSIDE the bodies
			return JAS_MOVED;
		}
		else
		{
			// the joint is now outside one (or both) bodies
			return JAS_MOVED_OUT;
		}
	}

	return JAS_NOT_MOVED;
}

//////////////////////////////////////////////////////////////////////////
// PhysicsJointWeld class
PhysicsJointWeld::PhysicsJointWeld( b2WeldJoint *joint, Material* material, Material* selectedMaterial, ulong id ) :
	PhysicsJoint(joint, PJT_Weld, material, selectedMaterial, id)
{
	SubPolygon *subpoly = createSubPolygon(DM_LINES);
	subpoly->addVertex(Vector2(-0.10f,-0.10f));
	subpoly->addVertex(Vector2( 0.10f, 0.10f));
	subpoly->addVertex(Vector2(-0.10f, 0.10f));
	subpoly->addVertex(Vector2( 0.10f,-0.10f));
	subpoly->setMaterial(_material);
	Polygon::setPosition(_joint->GetAnchorA());
	Polygon::setAngle(_joint->GetBodyA()->GetAngle());		
}

b2WeldJoint* PhysicsJointWeld::getBox2DWeldJoint()
{
	return static_cast<b2WeldJoint*>(_joint);
}

void PhysicsJointWeld::update(ulong timeSinceLastFrame)
{
	if(_selected)
	{
		// Do not sync the polygon with the b2d joint when it is selected
		return;
	}

	Polygon::setPosition(Vector2(_joint->GetAnchorA()));
	Polygon::setAngle(_joint->GetBodyA()->GetAngle());
}

PhySketch::JointAnchorsSituation PhysicsJointWeld::checkAnchorsSituation() const
{
	// TODO: improve anchor check (sometimes A and B anchors can be different)
	if(Vector2(_joint->GetAnchorA()) != _position || Vector2(_joint->GetAnchorB()) != _position)
	{
		// The joint polygon was manually moved. Check if the joint is still inside both bodies
		Polygon* bA = static_cast<Polygon*>(_joint->GetBodyA()->GetUserData());
		Polygon* bB = static_cast<Polygon*>(_joint->GetBodyB()->GetUserData());		
		if(bA->isPointInside(_position) && bB->isPointInside(_position))
		{
			// the joint was only moved INSIDE the bodies
			return JAS_MOVED;
		}
		else
		{
			// the joint is now outside one (or both) bodies
			return JAS_MOVED_OUT;
		}
	}

	return JAS_NOT_MOVED;
}

}

