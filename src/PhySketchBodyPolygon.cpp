#include "PhySketchBodyPolygon.h"
#include "Box2D\Dynamics\b2Body.h"
#include "Box2D\Dynamics\b2Fixture.h"
#include "Box2D\Collision\Shapes\b2CircleShape.h"
#include "Box2D\Collision\Shapes\b2PolygonShape.h"


namespace PhySketch
{

BodyPolygon::BodyPolygon(DrawingMode dm /*= DM_LINES*/) 
	: Polygon(dm, CS_Scene)
{
}

BodyPolygon::~BodyPolygon()
{
}

void BodyPolygon::update()
{
 	_position = _physicsBody->GetPosition();
	_angle = radiansToDegrees(_physicsBody->GetAngle());
}

const b2Body* BodyPolygon::getPhysicsBody( void ) const
{
	return(_physicsBody);
}

void BodyPolygon::setPhysicsBody( b2Body* body )
{
	_physicsBody = body;
	body->SetUserData(this);
	_position = _physicsBody->GetPosition();
	_angle = radiansToDegrees(_physicsBody->GetAngle());

	for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();
				
				std::vector<Vector2> circleVec = GetCircleVertices(circle->m_p, circle->m_radius, 180);
				size_t nVerts = circleVec.size();
				this->_vertexIndexes.reserve(nVerts);
				this->_vertices.reserve(nVerts);
				for (size_t i = 0; i < nVerts; i++)
				{
					this->addVertex(circleVec[i]);
				}
			}
			break;

		case b2Shape::e_polygon:
			{
				b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = poly->m_count;
				
				for (int32 i = 0; i < vertexCount; ++i)
				{
					addVertex(poly->m_vertices[i]);
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
	}
}

}

