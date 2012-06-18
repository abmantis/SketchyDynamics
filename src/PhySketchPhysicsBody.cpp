#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"
#include "Box2D\Dynamics\b2Body.h"
#include "Box2D\Dynamics\b2Fixture.h"
#include "Box2D\Collision\Shapes\b2CircleShape.h"
#include "Box2D\Collision\Shapes\b2PolygonShape.h"
#include "PhySketchRenderer.h"


namespace PhySketch
{

PhysicsBody::PhysicsBody()
{
	_body = nullptr;
	_needsPolygonUpdate = false;
}

PhysicsBody::PhysicsBody( b2Body *body ) 
	:_body(body)
{
	_needsPolygonUpdate = false;
	_body->SetUserData(this);
	reconstructPolygons();
}

PhysicsBody::~PhysicsBody()
{
	for (size_t i = 0; i < _polygons.size(); i++)
	{
		delete _polygons[i]; 
		_polygons[i] = nullptr;
	}
	for (size_t i = 0; i < _oldPolygons.size(); i++)
	{
		delete _oldPolygons[i]; 
		_oldPolygons[i] = nullptr;
	}
}

void PhysicsBody::update()
{
	size_t polyCount = _polygons.size();
	for (size_t i = 0; i < polyCount; i++)
	{
		_polygons[i]->setPosition(_body->GetPosition());
		_polygons[i]->setAngle(_body->GetAngle());
	}
}

b2Body* PhysicsBody::getBox2DBody()
{
	return(_body);
}

void PhysicsBody::setBox2DBody( b2Body* body )
{
	_body = body;
	_body->SetUserData(this);

	reconstructPolygons();
}

void PhysicsBody::reconstructPolygons()
{
	_needsPolygonUpdate = true;

	Vector2 position = _body->GetPosition();
	float angle = _body->GetAngle();
	Polygon *poly = nullptr;
	Polygon *linePoly = nullptr;

	// Remove old polygons	
	for (size_t i = 0; i < _polygons.size(); i++)
	{
		_oldPolygons.push_back(_polygons[i]);
	}
	_polygons.clear();

	for (b2Fixture* fixture = _body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{		
		poly = new Polygon(Polygon::VV_Static, Polygon::DM_TRIANGLE_FAN);
		poly->setPosition(position);
		poly->setAngle(angle);

		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

				std::vector<Vector2> circleVec = Polygon::GetCircleVertices(circle->m_p, circle->m_radius, 180);
				size_t nVerts = circleVec.size();
				for (size_t i = 0; i < nVerts; i++)
				{
					poly->addVertex(circleVec[i]);
				}
			}
			break;

		case b2Shape::e_polygon:
			{
				b2PolygonShape* box2dpoly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = box2dpoly->m_count;

				for (int32 i = 0; i < vertexCount; ++i)
				{
					poly->addVertex(box2dpoly->m_vertices[i]);
				}
			}
			break;
			// 		case b2Shape::e_edge:
			// 			{
			// 			}
			// 			break;
			// 
			// 		case b2Shape::e_chain:
			// 			{
			// 			}
			// 			break;
		default:
			break;
		}

		linePoly = new Polygon(*poly);
		linePoly->setDrawingMode(Polygon::DM_LINE_LOOP);
		_polygons.push_back(linePoly);

		_polygons.push_back(poly);
	}
}


}

