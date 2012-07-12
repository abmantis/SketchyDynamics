#include "PhySketchPhysicsBody.h"
#include <Box2D\Dynamics\b2Body.h>
#include <Box2D\Dynamics\b2Fixture.h>
#include <Box2D\Collision\Shapes\b2CircleShape.h>
#include <Box2D\Collision\Shapes\b2PolygonShape.h>
#include <Box2D\Collision\Shapes\b2ChainShape.h>
#include "PhySketchRenderer.h"
#include "PhySketchUtils.h"
#include "PhySketchVector2.h"


namespace PhySketch
{
PhysicsBody::PhysicsBody( b2Body *body, uint id, Material* fillMaterial, Material* lineMaterial, Material* selectedMaterial ) :	
	Polygon				(VV_Static, "PS_Body" + toString(ulong(id)) ),
	_body				(body),
	_selected			(false),
	_selectable			(true),
	_id					(id),
	_fillMaterial		(fillMaterial),
	_lineMaterial		(lineMaterial),
	_selectedMaterial	(selectedMaterial)
{
	_body->SetUserData(this);
	reconstructPolygon();
}

PhysicsBody::~PhysicsBody()
{
}

void PhysicsBody::update()
{
	Vector2 pos = _body->GetPosition();
	float angle = _body->GetAngle();

	Polygon::setPosition(pos);
	Polygon::setAngle(angle);	
}

b2Body* PhysicsBody::getBox2DBody()
{
	return(_body);
}

void PhysicsBody::setBox2DBody( b2Body* body )
{
	_body = body;
	_body->SetUserData(this);

	reconstructPolygon();
}

void PhysicsBody::reconstructPolygon()
{	
	// Remove old subpolygons
	clearSubPolygons();

	float lineWidth = 0.025f;
	
	//////////////////////////////////////////////////////////////////////////
	// Create new subpolygons
	SubPolygon* fillsubpoly = nullptr;
	SubPolygon* linesubpoly = nullptr;
	for (b2Fixture* fixture = _body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
				fillsubpoly = createSubPolygon(DM_TRIANGLE_FAN);
				fillsubpoly->setMaterial(_fillMaterial);
				linesubpoly = createSubPolygon(DM_TRIANGLE_STRIP);
				linesubpoly->setMaterial(_selected? _selectedMaterial : _lineMaterial);

				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

				std::vector<Vector2> circleVec = Polygon::GetCircleVertices(circle->m_p, circle->m_radius, 60);
				size_t nVerts = circleVec.size();
				Vector2 v;
				for (size_t i = 0; i < nVerts; i++)
				{
					v = circleVec[i];
					fillsubpoly->addVertex(v);
				}
				
// 				// Add extra lines in circle
// 				linesubpoly->addVertex(Vector2(circle->m_radius, 0.0f));
// 				linesubpoly->addVertex(Vector2(-circle->m_radius, 0.0f));
// 				linesubpoly->addVertex(Vector2(0.0f, 0.0f));
// 				linesubpoly->addVertex(Vector2(0.0f, circle->m_radius));
// 				linesubpoly->addVertex(Vector2(0.0f, -circle->m_radius));
// 				linesubpoly->addVertex(Vector2(0.0f, 0.0f));

				Vector2 prev_v = circleVec[0];
				Vector2 vecOrig, normal;
				for (size_t i = 1; i < nVerts; ++i)
				{
					v = circleVec[i];
					normal = Vector2::lineNormal(prev_v, v).normalised() * lineWidth;

					linesubpoly->addVertex(prev_v + normal, Vector2(0.0f, 1.0f));
					linesubpoly->addVertex(prev_v - normal, Vector2(1.0f, 1.0f));
					linesubpoly->addVertex(v + normal, Vector2(0.0f, 0.0f));
					linesubpoly->addVertex(v - normal, Vector2(1.0f, 0.0f));
					
					prev_v = v;
				}

				v = circleVec[0];
				normal = Vector2::lineNormal(prev_v, v).normalised() * lineWidth;

				linesubpoly->addVertex(prev_v + normal, Vector2(0.0f, 1.0f));
				linesubpoly->addVertex(prev_v - normal, Vector2(1.0f, 1.0f));
				linesubpoly->addVertex(v + normal, Vector2(0.0f, 0.0f));
				linesubpoly->addVertex(v - normal, Vector2(1.0f, 0.0f));
			}
			break;

		case b2Shape::e_polygon:
			{
				fillsubpoly = createSubPolygon(DM_TRIANGLE_FAN);
				fillsubpoly->setMaterial(_fillMaterial);
				linesubpoly = createSubPolygon(DM_TRIANGLES);
				linesubpoly->setMaterial(_selected? _selectedMaterial : _lineMaterial);

				b2PolygonShape* box2dpoly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = box2dpoly->m_count;

				Vector2 pt;
				for (int32 i = 0; i < vertexCount; ++i)
				{
					pt = box2dpoly->m_vertices[i];
					fillsubpoly->addVertex(pt);
				}

				Vector2 prev_pt = box2dpoly->m_vertices[0];
				Vector2 normal, prevNormal;
				Vector2 p1, p2, p3, p4, prev_p3, prev_p4;
				float angleBetweenEdges = 0.0f;

				prevNormal = Vector2::lineNormal(prev_pt, box2dpoly->m_vertices[1]).normalised() * lineWidth;
				for (int32 i = 1; i < vertexCount; ++i)
				{
					pt = box2dpoly->m_vertices[i];
					normal = Vector2::lineNormal(prev_pt, pt).normalised() * lineWidth;
					angleBetweenEdges = prevNormal.angleTo(normal);

					p1 = prev_pt + normal;
					p2 = prev_pt - normal;
					p3 = pt + normal;
					p4 = pt - normal;

					linesubpoly->addVertex(p1, Vector2(0.0f, 1.0f));
					linesubpoly->addVertex(p2, Vector2(1.0f, 1.0f));
					linesubpoly->addVertex(p3, Vector2(0.0f, 0.0f));
					linesubpoly->addVertex(p2, Vector2(1.0f, 1.0f));
					linesubpoly->addVertex(p4, Vector2(1.0f, 0.0f));
					linesubpoly->addVertex(p3, Vector2(0.0f, 0.0f));

					
					if( angleBetweenEdges < 0 )
					{
						linesubpoly->addVertex(prev_p3, Vector2(0.0f, 0.0f));
						linesubpoly->addVertex(prev_pt, Vector2(0.5f, 0.0f));
						linesubpoly->addVertex(p1,		Vector2(0.0f, 0.0f));
					}
					else if( angleBetweenEdges > 0 )
					{
						linesubpoly->addVertex(prev_p4, Vector2(1.0f, 0.0f));
						linesubpoly->addVertex(p2,		Vector2(1.0f, 1.0f));
						linesubpoly->addVertex(prev_pt, Vector2(0.5f, 0.0f));

					}

					
					prev_pt = pt;
					prevNormal = normal;
					prev_p3 = p3;
					prev_p4 = p4;
				}

				// Connect last and first vertices
				pt = box2dpoly->m_vertices[0];
				normal = Vector2::lineNormal(prev_pt, pt).normalised() * lineWidth;
				angleBetweenEdges = prevNormal.angleTo(normal);

				p1 = prev_pt + normal;
				p2 = prev_pt - normal;
				p3 = pt + normal;
				p4 = pt - normal;

				linesubpoly->addVertex(p1, Vector2(0.0f, 1.0f));
				linesubpoly->addVertex(p2, Vector2(1.0f, 1.0f));
				linesubpoly->addVertex(p3, Vector2(0.0f, 0.0f));
				linesubpoly->addVertex(p2, Vector2(1.0f, 1.0f));
				linesubpoly->addVertex(p4, Vector2(1.0f, 0.0f));
				linesubpoly->addVertex(p3, Vector2(0.0f, 0.0f));

				if( angleBetweenEdges < 0 )
				{
					linesubpoly->addVertex(prev_p3, Vector2(0.0f, 0.0f));
					linesubpoly->addVertex(prev_pt, Vector2(0.5f, 0.0f));
					linesubpoly->addVertex(p1,		Vector2(0.0f, 0.0f));
				}
				else if( angleBetweenEdges > 0 )
				{
					linesubpoly->addVertex(prev_p4, Vector2(1.0f, 0.0f));
					linesubpoly->addVertex(p2,		Vector2(1.0f, 1.0f));
					linesubpoly->addVertex(prev_pt, Vector2(0.5f, 0.0f));

				}

				// Add last corner
				prev_pt = pt;
				prevNormal = normal;
				prev_p3 = p3;
				prev_p4 = p4;

				pt = box2dpoly->m_vertices[1];
				normal = Vector2::lineNormal(prev_pt, pt).normalised() * lineWidth;
				angleBetweenEdges = prevNormal.angleTo(normal);

				p1 = prev_pt + normal;
				p2 = prev_pt - normal;

				if( angleBetweenEdges < 0 )
				{
					linesubpoly->addVertex(prev_p3, Vector2(0.0f, 0.0f));
					linesubpoly->addVertex(prev_pt, Vector2(0.5f, 0.0f));
					linesubpoly->addVertex(p1,		Vector2(0.0f, 0.0f));
				}
				else if( angleBetweenEdges > 0 )
				{
					linesubpoly->addVertex(prev_p4, Vector2(1.0f, 0.0f));
					linesubpoly->addVertex(p2,		Vector2(1.0f, 1.0f));
					linesubpoly->addVertex(prev_pt, Vector2(0.5f, 0.0f));

				}

			}
			break;
		case b2Shape::e_chain:
			{
				linesubpoly = createSubPolygon(DM_LINE_LOOP);
				linesubpoly->setMaterial(_selected? _selectedMaterial : _lineMaterial);

				b2ChainShape* box2dchain = (b2ChainShape*)fixture->GetShape();
				int32 vertexCount = box2dchain->m_count;

				for (int32 i = 0; i < vertexCount; ++i)
				{
					linesubpoly->addVertex(box2dchain->m_vertices[i]);
				}
			}
			break;
		case b2Shape::e_edge:
			{
				throw std::exception("Edge bodies not implemented.");
			}
			break;

		default:
			break;
		}
	}
}

Material* PhysicsBody::getFillMaterial( void ) const
{
	return(_fillMaterial);
}

void PhysicsBody::setFillMaterial( Material* fillMaterial )
{
	_fillMaterial = fillMaterial;
}

Material* PhysicsBody::getLineMaterial( void ) const
{
	return(_lineMaterial);
}

void PhysicsBody::setLineMaterial( Material* lineMaterial )
{
	_lineMaterial = lineMaterial;
}

Material* PhysicsBody::getSelectedMaterial( void ) const
{
	return _selectedMaterial;
}

void PhysicsBody::setSelectedMaterial( Material* material )
{
	_selectedMaterial = material;
}

bool PhysicsBody::isSelected() const
{
	return _selected;
}

bool PhysicsBody::isSelectable() const
{
	return _selectable;
}

void PhysicsBody::translate( const Vector2& amount )
{
	Vector2 pos = _body->GetPosition();
	pos += amount;
	_body->SetTransform(pos.tob2Vec2(), _body->GetAngle());
	Polygon::setPosition(pos);
}

void PhysicsBody::rotate( const float& angle )
{
	_body->SetTransform(_body->GetPosition(), _body->GetAngle() + angle);
	Polygon::rotate(angle);
}

void PhysicsBody::rotateAroundPoint( float angle, const Vector2& rotationPoint )
{
	Vector2 pos = _body->GetPosition();
	Vector2 rotatedDist = Vector2::Rotate(pos - rotationPoint, angle);
	pos = rotationPoint + rotatedDist;

 	_body->SetTransform(pos.tob2Vec2(), _body->GetAngle() + angle);
	Polygon::setPosition(pos);
	Polygon::rotate(angle);
}

void PhysicsBody::scale( const Vector2& factor )
{
	updateAABB();
	Vector2 newAABBSize = _aabb.getSize()*_scale*factor;
	if(newAABBSize.x < 0.01f || newAABBSize.y < 0.01f )
		return;

	b2Fixture* node_fixture = _body->GetFixtureList();
	b2Fixture* fixture;
	while(node_fixture) 
	{
		fixture = node_fixture;
		node_fixture = node_fixture->GetNext();

		b2FixtureDef newFixtureDef;		
		newFixtureDef.density = fixture->GetDensity();
		newFixtureDef.friction = fixture->GetFriction();
		newFixtureDef.restitution = fixture->GetRestitution();	
		newFixtureDef.filter = fixture->GetFilterData();
		newFixtureDef.isSensor = fixture->IsSensor();
		newFixtureDef.userData = fixture->GetUserData();

		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
				b2CircleShape newCircleShape;
				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();				
				newCircleShape.m_p = circle->m_p;
				newCircleShape.m_radius = circle->m_radius * factor.x;
				
				newFixtureDef.shape = &newCircleShape;		

				_body->DestroyFixture(fixture);
				_body->CreateFixture(&newFixtureDef);
			}
			break;

		case b2Shape::e_polygon:
			{
				b2PolygonShape newShape;
				b2PolygonShape* box2dpoly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = box2dpoly->m_count;

				b2Vec2 *vertices = new b2Vec2[vertexCount];
				for (int32 i = 0; i < vertexCount; ++i)
				{
					b2Vec2 pos = box2dpoly->m_vertices[i];
					pos.x *= factor.x;
					pos.y *= factor.y;

					vertices[i] = pos;
				}
				newShape.Set(vertices, vertexCount);
				delete[] vertices;
				newFixtureDef.shape = &newShape;

				_body->DestroyFixture(fixture);
				_body->CreateFixture(&newFixtureDef);
			}
			break;
		case b2Shape::e_edge:
			{
				throw std::exception("Edge body scaling is not implemented.");
			}
			break;

		case b2Shape::e_chain:
			{
				throw std::exception("Chain body scaling is not implemented.");
			}
			break;
		default:
			break;
		}		
	}
	
	_body->ResetMassData();

	Polygon::scale(factor);
}

void PhysicsBody::setAngle( float angle )
{
	_body->SetTransform(_body->GetPosition(), angle);
	Polygon::setAngle(angle);
}

void PhysicsBody::setPosition( const Vector2& position )
{
	_body->SetTransform(position.tob2Vec2(), _body->GetAngle());
	Polygon::setPosition(position);
}

void PhysicsBody::setScale( const Vector2& scale )
{
	throw std::exception("The method or operation is not implemented.");
}


}

