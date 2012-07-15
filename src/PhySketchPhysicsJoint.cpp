#include "PhySketchPhysicsJoint.h"
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/Joints/b2DistanceJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include "PhySketchUtils.h"

namespace PhySketch
{
PhysicsJoint::PhysicsJoint( b2Joint *joint, PhysicsJointType type, Material* material, Material* selectedMaterial, ulong id ) :
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
	_poly = new Polygon(VV_Static, "PS_Joint" + toString(ulong(id)));
	_poly->CreateCircleSubPolygon(DM_LINE_LOOP, Vector2::ZERO_XY, 0.10f, 80)->setMaterial(_material);
	_poly->setPosition(_joint->GetAnchorA());

	_poly->setUserType(PHYSKETCH_POLYGON_UTYPE_PHYJOINT);
	_poly->setUserData(this);

	// create a square only for click detection
	_poly->CreateSquareSubPolygon(DM_TRIANGLE_FAN, Vector2(0.1f, 0.1f))->setVisible(false);
}

PhysicsJointRevolute::~PhysicsJointRevolute()
{
	delete _poly;
	_poly = nullptr;
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
	
	_poly->setPosition(Vector2(_joint->GetAnchorA()));
}

PhySketch::JointAnchorsSituation PhysicsJointRevolute::checkAnchorsSituation() const
{
	Vector2 jointPos = _poly->getPosition();
	// TODO: improve anchor check (sometimes A and B anchors can be different)
	if(Vector2(_joint->GetAnchorA()) != jointPos || Vector2(_joint->GetAnchorB()) != jointPos)
	{		
		// The joint polygon was manually moved. Check if the joint is still inside both bodies
		Polygon* bA = static_cast<Polygon*>(_joint->GetBodyA()->GetUserData());
		Polygon* bB = static_cast<Polygon*>(_joint->GetBodyB()->GetUserData());		
		if(bA->isPointInside(jointPos) && bB->isPointInside(jointPos))
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

void PhysicsJointRevolute::setPosition( const Vector2& position )
{
	PHYSKETCH_ASSERT(_selected && "Cannot setPosition on an unselected joint");
	_poly->setPosition(position);
}

void PhysicsJointRevolute::translate( const Vector2& amount )
{
	PHYSKETCH_ASSERT(_selected && "Cannot translate an unselected joint");
	_poly->translate(amount);
}

void PhysicsJointRevolute::rotateAroundPoint( float angle, const Vector2& rotationPoint )
{
	PHYSKETCH_ASSERT(_selected && "Cannot rotateAroundPoint an unselected joint");
	_poly->rotateAroundPoint(angle, rotationPoint);
}

bool PhysicsJointRevolute::isPointInside( const Vector2& pt ) const
{
	return _poly->getTransformedAABB(true).isPointInside(pt);
}

void PhysicsJointRevolute::select()
{
	_selected = true;
	_poly->setMaterial(_selectedMaterial);
}

void PhysicsJointRevolute::unselect()
{
	_selected = false;
	_poly->setMaterial(_material);
}

Vector2 PhysicsJointRevolute::getPosition() const
{
	return _poly->getPosition();
}

//////////////////////////////////////////////////////////////////////////
// PhysicsJointWeld class
PhysicsJointWeld::PhysicsJointWeld( b2WeldJoint *joint, Material* material, Material* selectedMaterial, ulong id ) :
	PhysicsJoint(joint, PJT_Weld, material, selectedMaterial, id)
{
	_poly = new Polygon(VV_Static, "PS_Joint" + toString(ulong(id)));
	
	SubPolygon *subpoly = _poly->createSubPolygon(DM_LINES);
	subpoly->addVertex(Vector2(-0.10f,-0.10f));
	subpoly->addVertex(Vector2( 0.10f, 0.10f));
	subpoly->addVertex(Vector2(-0.10f, 0.10f));
	subpoly->addVertex(Vector2( 0.10f,-0.10f));
	subpoly->setMaterial(_material);
	_poly->setPosition(_joint->GetAnchorA());
	_poly->setAngle(_joint->GetBodyA()->GetAngle());		

	_poly->setUserType(PHYSKETCH_POLYGON_UTYPE_PHYJOINT);
	_poly->setUserData(this);

	// create a square only for click detection
	subpoly = _poly->CreateSquareSubPolygon(DM_TRIANGLE_FAN, Vector2(0.1f, 0.1f));
	subpoly->setVisible(false);
}
	
PhysicsJointWeld::~PhysicsJointWeld()
{
	delete _poly;
	_poly = nullptr;
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

	_poly->setPosition(Vector2(_joint->GetAnchorA()));
	_poly->setAngle(_joint->GetBodyA()->GetAngle());
}

PhySketch::JointAnchorsSituation PhysicsJointWeld::checkAnchorsSituation() const
{
	Vector2 jointPos = _poly->getPosition();
	// TODO: improve anchor check (sometimes A and B anchors can be different)
	if(Vector2(_joint->GetAnchorA()) != jointPos || Vector2(_joint->GetAnchorB()) != jointPos)
	{
		// The joint polygon was manually moved. Check if the joint is still inside both bodies
		Polygon* bA = static_cast<Polygon*>(_joint->GetBodyA()->GetUserData());
		Polygon* bB = static_cast<Polygon*>(_joint->GetBodyB()->GetUserData());		
		if(bA->isPointInside(jointPos) && bB->isPointInside(jointPos))
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

void PhysicsJointWeld::setPosition( const Vector2& position )
{
	PHYSKETCH_ASSERT(_selected && "Cannot setPosition on an unselected joint");
	_poly->setPosition(position);
}

void PhysicsJointWeld::translate( const Vector2& amount )
{
	PHYSKETCH_ASSERT(_selected && "Cannot translate an unselected joint");
	_poly->translate(amount);
}

void PhysicsJointWeld::rotateAroundPoint( float angle, const Vector2& rotationPoint )
{
	PHYSKETCH_ASSERT(_selected && "Cannot rotateAroundPoint an unselected joint");
	_poly->rotateAroundPoint(angle, rotationPoint);
}

bool PhysicsJointWeld::isPointInside( const Vector2& pt ) const
{
	return _poly->getTransformedAABB(true).isPointInside(pt);
}

void PhysicsJointWeld::select()
{
	_selected = true;
	_poly->setMaterial(_selectedMaterial);
}

void PhysicsJointWeld::unselect()
{
	_selected = false;
	_poly->setMaterial(_material);
}

Vector2 PhysicsJointWeld::getPosition() const
{
	return _poly->getPosition();
}

//////////////////////////////////////////////////////////////////////////
// PhysicsJointDistance class
PhysicsJointDistance::PhysicsJointDistance( b2DistanceJoint *joint, Material* material, Material* selectedMaterial, ulong id ) :
	PhysicsJoint(joint, PJT_Distance, material, selectedMaterial, id)
{	
	Vector2 anchorA = _joint->GetAnchorA();
	Vector2 anchorB = _joint->GetAnchorB();
	float distance = anchorA.distanceTo(anchorB);

	_poly = new Polygon(VV_Static, "PS_Joint" + toString(ulong(id)));

	SubPolygon *subpoly = _poly->createSubPolygon(DM_LINE_STRIP);

	int nrSegments = 5*distance; 
	float increment = 1.0f / nrSegments;
	Vector2 p1(0.0f, 0.0f);
	Vector2 p2 = Vector2(0.25f / (float)nrSegments, 0.20f);
	Vector2 p3 = Vector2(0.75f / (float)nrSegments,-0.20f);
	Vector2 p4 = Vector2(1.00f / (float)nrSegments, 0.0f);
	subpoly->addVertex(p1);
	for (int i = 0; i < nrSegments; ++i)
	{
		subpoly->addVertex(p2);
		subpoly->addVertex(p3);
		subpoly->addVertex(p4);

		p2.x += increment;
		p3.x += increment;
		p4.x += increment;
	}	
// 	subpoly->addVertex(Vector2( 0.0f, 0.0f));
// 	subpoly->addVertex(Vector2( 1.0f, 0.0f));
	
// 	// Create anchor point circles
// 	Polygon::CreateCircleSubPolygon(DM_TRIANGLE_FAN, Vector2::ZERO_XY, 0.10f, 80);
// 	Polygon::CreateCircleSubPolygon(DM_TRIANGLE_FAN, Vector2::UNIT_X, 0.10f, 80);
	
	_poly->setMaterial(_material);
		
	_poly->setPosition(anchorA);
	_poly->setAngle(Vector2::lineAngle(anchorA, anchorB));
	_poly->setScale(Vector2(distance, 1.0f));

	_poly->setUserType(PHYSKETCH_POLYGON_UTYPE_PHYJOINT);
	_poly->setUserData(this);
}

PhysicsJointDistance::~PhysicsJointDistance()
{
	delete _poly;
	_poly = nullptr;
}

b2DistanceJoint* PhysicsJointDistance::getBox2DDistanceJoint()
{
	return static_cast<b2DistanceJoint*>(_joint);
}

void PhysicsJointDistance::update( ulong timeSinceLastFrame )
{
	if(_selected)
	{
		// Do not sync the polygon with the b2d joint when it is selected
		return;
	}

	Vector2 anchorA = _joint->GetAnchorA();
	Vector2 anchorB = _joint->GetAnchorB();	
	_poly->setPosition(anchorA);
	_poly->setAngle(Vector2::lineAngle(anchorA, anchorB));
	_poly->setScale(Vector2(anchorA.distanceTo(anchorB), 1.0f));
}

PhySketch::JointAnchorsSituation PhysicsJointDistance::checkAnchorsSituation() const
{
	return JAS_NOT_MOVED;
}

void PhysicsJointDistance::setPosition( const Vector2& position )
{
	PHYSKETCH_ASSERT(_selected && "Cannot setPosition on an unselected joint");
	_poly->setPosition(position);
}

void PhysicsJointDistance::translate( const Vector2& amount )
{
	PHYSKETCH_ASSERT(_selected && "Cannot translate an unselected joint");
	_poly->translate(amount);
}

void PhysicsJointDistance::rotateAroundPoint( float angle, const Vector2& rotationPoint )
{
	PHYSKETCH_ASSERT(_selected && "Cannot rotateAroundPoint an unselected joint");
	_poly->rotateAroundPoint(angle, rotationPoint);
}

bool PhysicsJointDistance::isPointInside( const Vector2& pt ) const
{
	return _poly->getTransformedAABB(true).isPointInside(pt);
}

void PhysicsJointDistance::select()
{
	_selected = true;
	_poly->setMaterial(_selectedMaterial);
}

void PhysicsJointDistance::unselect()
{
	_selected = false;
	_poly->setMaterial(_material);
}

}

