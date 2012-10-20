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
	_selected			(false),
	_validSituation		(true)
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

PhySketch::PhysicsJointType PhysicsJoint::getType() const
{
	return _pjt;
}


//////////////////////////////////////////////////////////////////////////
// PhysicsJointRevolute class
PhysicsJointRevolute::PhysicsJointRevolute( b2RevoluteJoint *joint, Material* material, Material* selectedMaterial, ulong id ) :
	PhysicsJoint(joint, PJT_Revolute, material, selectedMaterial, id)
{
	_poly = new AnimatedPolygon(VV_Static, "PS_Joint" + toString(ulong(id)));
	_poly->CreateCircleSubPolygon(DM_LINE_LOOP, Vector2::ZERO_XY, 0.10f, 80)->setMaterial(_material);
	_poly->Polygon::setPosition(_joint->GetAnchorA());

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
	
	_poly->Polygon::setPosition(Vector2(_joint->GetAnchorA()));
}

PhySketch::JointAnchorsSituation PhysicsJointRevolute::checkAnchorsSituation(bool updateVisual) 
{
	JointAnchorsSituation jas = JAS_NOT_MOVED;
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
			jas = JAS_MOVED;
		}
		else
		{
			// the joint is now outside one (or both) bodies
			jas = JAS_MOVED_OUT;
		}
	}

	if(updateVisual)
	{
		if(_validSituation == true)
		{
			if(jas == JAS_MOVED_OUT)
			{
				_poly->blink(500, 1, 250, 500);
			}
		}
		else if(jas == JAS_MOVED)
		{
			_poly->stopBlink(true);
		}
	}

	if(jas == JAS_MOVED_OUT)
	{
		_validSituation = false;
	}
	else
	{
		_validSituation = true;
	}

	return jas;
}

void PhysicsJointRevolute::setPosition( const Vector2& position )
{
	PHYSKETCH_ASSERT(_selected && "Cannot setPosition on an unselected joint");
	_poly->Polygon::setPosition(position);
}

void PhysicsJointRevolute::translate( const Vector2& amount )
{
	PHYSKETCH_ASSERT(_selected && "Cannot translate an unselected joint");
	_poly->Polygon::translate(amount);
}

void PhysicsJointRevolute::rotateAroundPoint( float angle, const Vector2& rotationPoint )
{
	PHYSKETCH_ASSERT(_selected && "Cannot rotateAroundPoint an unselected joint");
	_poly->rotateAroundPoint(angle, rotationPoint);
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
	_poly = new AnimatedPolygon(VV_Static, "PS_Joint" + toString(ulong(id)));
	
	SubPolygon *subpoly = _poly->createSubPolygon(DM_LINES);
	subpoly->addVertex(Vector2(-0.10f,-0.10f));
	subpoly->addVertex(Vector2( 0.10f, 0.10f));
	subpoly->addVertex(Vector2(-0.10f, 0.10f));
	subpoly->addVertex(Vector2( 0.10f,-0.10f));
	subpoly->setMaterial(_material);
	_poly->Polygon::setPosition(_joint->GetAnchorA());
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

	_poly->Polygon::setPosition(Vector2(_joint->GetAnchorA()));
	_poly->setAngle(_joint->GetBodyA()->GetAngle());
}

PhySketch::JointAnchorsSituation PhysicsJointWeld::checkAnchorsSituation(bool updateVisual)
{
	JointAnchorsSituation jas = JAS_NOT_MOVED;
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
			jas = JAS_MOVED;
		}
		else
		{
			// the joint is now outside one (or both) bodies
			jas = JAS_MOVED_OUT;
		}
	}

	if(updateVisual)
	{
		if(_validSituation == true)
		{
			if(jas == JAS_MOVED_OUT)
			{
				_poly->blink(500, 1, 250, 500);
			}
		}
		else if(jas == JAS_MOVED)
		{
			_poly->stopBlink(true);
		}
	}

	if(jas == JAS_MOVED_OUT)
	{
		_validSituation = false;
	}
	else
	{
		_validSituation = true;
	}

	return jas;
}

void PhysicsJointWeld::setPosition( const Vector2& position )
{
	PHYSKETCH_ASSERT(_selected && "Cannot setPosition on an unselected joint");
	_poly->Polygon::setPosition(position);
}

void PhysicsJointWeld::translate( const Vector2& amount )
{
	PHYSKETCH_ASSERT(_selected && "Cannot translate an unselected joint");
	_poly->Polygon::translate(amount);
}

void PhysicsJointWeld::rotateAroundPoint( float angle, const Vector2& rotationPoint )
{
	PHYSKETCH_ASSERT(_selected && "Cannot rotateAroundPoint an unselected joint");
	_poly->rotateAroundPoint(angle, rotationPoint);
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

	std::string jointName = "PS_Joint" + toString(ulong(id));

	_zigZagPoly = new AnimatedPolygon(VV_Static, jointName + "zigzag");

	SubPolygon *subpoly = _zigZagPoly->createSubPolygon(DM_LINE_STRIP);

	int nrSegments = (int)(distance*5.0f);
	float increment = 1.0f / nrSegments;
	Vector2 p1(0.0f, 0.0f);
	Vector2 p2 = Vector2(0.25f / (float)nrSegments, 0.20f);
	Vector2 p3 = Vector2(0.75f / (float)nrSegments,-0.20f);
	Vector2 p4 = Vector2(1.00f / (float)nrSegments, 0.0f);
	Vector2 rect_p1, rect_p2, rect_p3, rect_p4;

	subpoly->addVertex(p1);
	for (int i = 0; i < nrSegments; ++i)
	{
		subpoly->addVertex(p2);
		subpoly->addVertex(p3);
		subpoly->addVertex(p4);

		p1.x += increment;
		p2.x += increment;
		p3.x += increment;
		p4.x += increment;
	}	

	_zigZagPoly->CreateSquareSubPolygon(DM_TRIANGLE_FAN, Vector2(0.5f, 0.2f), Vector2(0.5f, 0.0f))->setVisible(false);
		
	_material->setColor(Color(1.0f, 0,0, 0.5f));
	_zigZagPoly->setMaterial(_material);
		
	_zigZagPoly->Polygon::setPosition(anchorA);
	_zigZagPoly->setAngle(Vector2::lineAngle(anchorA, anchorB));
	_zigZagPoly->setScale(Vector2(distance, 1.0f));
	_zigZagPoly->setUserType(PHYSKETCH_POLYGON_UTYPE_PHYJOINT);
	_zigZagPoly->setUserData(this);

	// Create anchor point circles
	_circlePolyA = new AnimatedPolygon(VV_Static, jointName + "circleA");
	_circlePolyB = new AnimatedPolygon(VV_Static, jointName + "circleB");
	_circlePolyA->CreateCircleSubPolygon(DM_TRIANGLE_FAN, Vector2::ZERO_XY, 0.045f, 20);
	_circlePolyB->CreateCircleSubPolygon(DM_TRIANGLE_FAN, Vector2::ZERO_XY, 0.045f, 20);

	_circlePolyA->setMaterial(_material);
	_circlePolyB->setMaterial(_material);
	_circlePolyA->Polygon::setPosition(anchorA);
	_circlePolyB->Polygon::setPosition(anchorB);
	_circlePolyA->setUserType(PHYSKETCH_POLYGON_UTYPE_PHYJOINT);
	_circlePolyA->setUserData(this);
	_circlePolyB->setUserType(PHYSKETCH_POLYGON_UTYPE_PHYJOINT);
	_circlePolyB->setUserData(this);
}

PhysicsJointDistance::~PhysicsJointDistance()
{
	delete _zigZagPoly;
	_zigZagPoly = nullptr;
	delete _circlePolyA;	
	_circlePolyA = nullptr;
	delete _circlePolyB;
	_circlePolyB = nullptr;
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
	_zigZagPoly->Polygon::setPosition(anchorA);
	_zigZagPoly->setAngle(Vector2::lineAngle(anchorA, anchorB));
	_zigZagPoly->setScale(Vector2(anchorA.distanceTo(anchorB), 1.0f));

	_circlePolyA->Polygon::setPosition(anchorA);
	_circlePolyB->Polygon::setPosition(anchorB);
}

PhySketch::JointAnchorsSituation PhysicsJointDistance::checkAnchorsSituation(bool updateVisual)
{
	JointAnchorsSituation jas = JAS_NOT_MOVED;
	Vector2 anchorA = _circlePolyA->getPosition();
	Vector2 anchorB = _circlePolyB->getPosition();
	if(Vector2(_joint->GetAnchorA()) != anchorA || Vector2(_joint->GetAnchorB()) != anchorB)
	{
		// The joint polygon was manually moved. Check if the joint is still inside both bodies
		Polygon* bA = static_cast<Polygon*>(_joint->GetBodyA()->GetUserData());
		Polygon* bB = static_cast<Polygon*>(_joint->GetBodyB()->GetUserData());		
		if(bA->isPointInside(anchorA) && bB->isPointInside(anchorB))
		{
			// the joint was only moved INSIDE the bodies
			jas = JAS_MOVED;
		}
		else
		{
			// the joint is now outside one (or both) bodies
			jas = JAS_MOVED_OUT;
		}
	}

	if(updateVisual)
	{
		if(_validSituation == true)
		{
			if(jas == JAS_MOVED_OUT)
			{
				_zigZagPoly->blink(500, 1, 250, 500);
				_circlePolyA->blink(500, 1, 250, 500);
				_circlePolyB->blink(500, 1, 250, 500);
			}
		}
		else if(jas == JAS_MOVED)
		{
			_zigZagPoly->stopBlink(true);
			_circlePolyA->stopBlink(true);
			_circlePolyB->stopBlink(true);
		}
	}

	if(jas == JAS_MOVED_OUT)
	{
		_validSituation = false;
	}
	else
	{
		_validSituation = true;
	}

	return jas;
}

void PhysicsJointDistance::setPosition( const Vector2& position )
{
	/*PHYSKETCH_ASSERT(_selected && "Cannot setPosition on an unselected joint");
	_zigZagPoly->setPosition(position);
	_circlePolyA->setPosition(position);
	_circlePolyB->setPosition(position);*/
	throw std::exception("The method or operation is not implemented.");
}


void PhysicsJointDistance::setPositions( const Vector2& anchorA, const Vector2& anchorB )
{
	PHYSKETCH_ASSERT(_selected && "Cannot setPositions on an unselected joint");

	_zigZagPoly->Polygon::setPosition(anchorA);
	_zigZagPoly->setAngle(Vector2::lineAngle(anchorA, anchorB));
	_zigZagPoly->setScale(Vector2(anchorA.distanceTo(anchorB), 1.0f));

	_circlePolyA->Polygon::setPosition(anchorA);
	_circlePolyB->Polygon::setPosition(anchorB);
}


void PhysicsJointDistance::translate( const Vector2& amount )
{
	PHYSKETCH_ASSERT(_selected && "Cannot translate an unselected joint");
	_zigZagPoly->Polygon::translate(amount);
	_circlePolyA->Polygon::translate(amount);
	_circlePolyB->Polygon::translate(amount);
}

void PhysicsJointDistance::rotateAroundPoint( float angle, const Vector2& rotationPoint )
{
	PHYSKETCH_ASSERT(_selected && "Cannot rotateAroundPoint an unselected joint");
	_zigZagPoly->rotateAroundPoint(angle, rotationPoint);
	_circlePolyA->rotateAroundPoint(angle, rotationPoint);
	_circlePolyB->rotateAroundPoint(angle, rotationPoint);
}

void PhysicsJointDistance::select()
{
	_selected = true;
	_zigZagPoly->setMaterial(_selectedMaterial);
	_circlePolyA->setMaterial(_selectedMaterial);
	_circlePolyB->setMaterial(_selectedMaterial);
}

void PhysicsJointDistance::unselect()
{
	_selected = false;
	_zigZagPoly->setMaterial(_material);
	_circlePolyA->setMaterial(_material);
	_circlePolyB->setMaterial(_material);
}

PhySketch::Vector2 PhysicsJointDistance::getPositionA() const
{
	return _circlePolyA->getPosition();
}

PhySketch::Vector2 PhysicsJointDistance::getPositionB() const
{
	return _circlePolyB->getPosition();
}

}

