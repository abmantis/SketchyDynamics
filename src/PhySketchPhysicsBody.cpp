#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"
#include "Box2D\Dynamics\b2Body.h"
#include "Box2D\Dynamics\b2Fixture.h"
#include "Box2D\Collision\Shapes\b2CircleShape.h"
#include "Box2D\Collision\Shapes\b2PolygonShape.h"
#include "PhySketchRenderer.h"
#include "PhySketchUtils.h"


namespace PhySketch
{
PhysicsBody::PhysicsBody( b2Body *body, uint id ) :
	_body(body),
	_polygon(nullptr),
	_needsPolygonUpdate(false),
	_selected(false),
	_selectable(true),
	_id(id)
{
	_fillMaterial.setColor(Color(0.7f, 0.7f, 0.8f, 0.0f));
	_lineMaterial.setColor(Color(0.3f, 0.3f, 1.0f, 0.0f));
	_selectedMaterial.setColor(Color(1.0f, 0.5f, 0.5f, 0.0f));

	_body->SetUserData(this);
	reconstructPolygons();
}

PhysicsBody::~PhysicsBody()
{
	delete _polygon;
	_polygon = nullptr;

	for (size_t i = 0; i < _oldPolygons.size(); i++)
	{
		delete _oldPolygons[i]; 
		_oldPolygons[i] = nullptr;
	}
}

void PhysicsBody::update()
{
	Vector2 pos = _body->GetPosition();
	float angle = _body->GetAngle();

	_polygon->setPosition(pos);
	_polygon->setAngle(angle);	
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

	std::string polyName = "PS_Body" + toString(_id);
	
	// Remove old polygon	
	if(_polygon)
		_oldPolygons.push_back(_polygon);	
	
	//////////////////////////////////////////////////////////////////////////
	// Create new polygon
	_polygon = new Polygon(VV_Static, polyName);
	SubPolygon* fillsubpoly = nullptr;
	SubPolygon* linesubpoly = nullptr;
	for (b2Fixture* fixture = _body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{	
		_polygon->setPosition(position);
		_polygon->setAngle(angle);

		fillsubpoly = _polygon->createSubPolygon(DM_TRIANGLE_FAN);
		fillsubpoly->SetMaterial(_fillMaterial);
		linesubpoly = _polygon->createSubPolygon(DM_LINE_LOOP);
		linesubpoly->SetMaterial(_selected? _selectedMaterial : _lineMaterial);

		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
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

PhySketch::Vector2 PhysicsBody::translate( Vector2 translation )
{
	Vector2 pos = _body->GetPosition();
	pos += translation;
	_body->SetTransform(pos.tob2Vec2(), _body->GetAngle());
	return pos;
}

void PhysicsBody::rotateAroundPoint( float angle, Vector2 rotationPoint )
{
	Vector2 pos = _body->GetPosition();
	Vector2 rotatedDist = Vector2::Rotate(pos - rotationPoint, angle);
	pos = rotationPoint + rotatedDist;

 	_body->SetTransform(pos.tob2Vec2(), _body->GetAngle() + angle); 	
}

void PhysicsBody::scale( Vector2 factor )
{
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


	_polygon->scale(factor);

	_body->ResetMassData();
}


}

