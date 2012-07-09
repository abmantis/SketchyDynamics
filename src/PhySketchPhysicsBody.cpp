#include "PhySketchPhysicsBody.h"
#include <Box2D\Dynamics\b2Body.h>
#include <Box2D\Dynamics\b2Fixture.h>
#include <Box2D\Collision\Shapes\b2CircleShape.h>
#include <Box2D\Collision\Shapes\b2PolygonShape.h>
#include <Box2D\Collision\Shapes\b2ChainShape.h>
#include "PhySketchRenderer.h"
#include "PhySketchUtils.h"


namespace PhySketch
{
PhysicsBody::PhysicsBody( b2Body *body, uint id ) :	
	Polygon(VV_Static, "PS_Body" + toString(ulong(id)) ),
	_body(body),
	_selected(false),
	_selectable(true),
	_id(id)
{
	_fillMaterial.setColor(Color(0.7f, 0.7f, 0.8f, 0.0f));
	_lineMaterial.setColor(Color(0.3f, 0.3f, 1.0f, 0.0f));
	_selectedMaterial.setColor(Color(1.0f, 0.5f, 0.5f, 0.0f));

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
				fillsubpoly->SetMaterial(_fillMaterial);
				linesubpoly = createSubPolygon(DM_LINE_LOOP);
				linesubpoly->SetMaterial(_selected? _selectedMaterial : _lineMaterial);

				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

				std::vector<Vector2> circleVec = Polygon::GetCircleVertices(circle->m_p, circle->m_radius, 180);
				size_t nVerts = circleVec.size();
				Vector2 v;
				for (size_t i = 0; i < nVerts; i++)
				{
					v = circleVec[i];
					fillsubpoly->addVertex(v);
					linesubpoly->addVertex(v);
				}
				
				// Add extra lines in circle
				linesubpoly->addVertex(Vector2(circle->m_radius, 0.0f));
				linesubpoly->addVertex(Vector2(-circle->m_radius, 0.0f));
				linesubpoly->addVertex(Vector2(0.0f, 0.0f));
				linesubpoly->addVertex(Vector2(0.0f, circle->m_radius));
				linesubpoly->addVertex(Vector2(0.0f, -circle->m_radius));
				linesubpoly->addVertex(Vector2(0.0f, 0.0f));
			}
			break;

		case b2Shape::e_polygon:
			{
				fillsubpoly = createSubPolygon(DM_TRIANGLE_FAN);
				fillsubpoly->SetMaterial(_fillMaterial);
				linesubpoly = createSubPolygon(DM_LINE_LOOP);
				linesubpoly->SetMaterial(_selected? _selectedMaterial : _lineMaterial);

				b2PolygonShape* box2dpoly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = box2dpoly->m_count;

				Vector2 v;
				for (int32 i = 0; i < vertexCount; ++i)
				{
					v = box2dpoly->m_vertices[i];
					fillsubpoly->addVertex(v);
					linesubpoly->addVertex(v);
				}
			}
			break;
		case b2Shape::e_chain:
			{
				linesubpoly = createSubPolygon(DM_LINE_LOOP);
				linesubpoly->SetMaterial(_selected? _selectedMaterial : _lineMaterial);

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

const Material& PhysicsBody::getFillMaterial( void ) const
{
	return(_fillMaterial);
}

void PhysicsBody::setFillMaterial( const Material& fillMaterial )
{
	_fillMaterial = fillMaterial;
}

const Material& PhysicsBody::getLineMaterial( void ) const
{
	return(_lineMaterial);
}

void PhysicsBody::setLineMaterial( const Material& lineMaterial )
{
	_lineMaterial = lineMaterial;
}

const Material& PhysicsBody::getSelectedMaterial( void ) const
{
	return _selectedMaterial;
}

void PhysicsBody::setSelectedMaterial( const Material& material )
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

