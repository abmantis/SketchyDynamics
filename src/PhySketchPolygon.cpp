#include "PhySketchPolygon.h"

namespace PhySketch
{

Polygon::Polygon(CoordinateSystem cs /*= CS_Scene*/) : _angle(0), _position(0, 0), _scale(1, 1), _coordSystem(cs)
{
}

Polygon::~Polygon()
{
}

double Polygon::getAngle( void ) const
{
	return(_angle);
}

void Polygon::setAngle( double angle )
{
	_angle = angle;
}

const Vector2& Polygon::getPosition( void ) const
{
	return(_position);
}

void Polygon::setPosition( const Vector2& position )
{
	_position = position;
}

const Vector2& Polygon::getScale( void ) const
{
	return(_scale);
}

void Polygon::setScale( const Vector2& scale )
{
	_scale = scale;
}

void Polygon::translate( const Vector2& amount )
{
	_position += amount;
}

void Polygon::addVertex( const Vector2& vertex )
{
	_vertexIndexes.push_back(_vertexIndexes.size());
	_vertices.push_back(vertex);	
}

const Polygon::CoordinateSystem& Polygon::getCoordinateSystem() const
{
	return _coordSystem;
}

Polygon* Polygon::getSquare()
{
	Polygon *poly = new Polygon(CS_Scene);
	poly->addVertex(Vector2(-0.5f, -0.5f));
	poly->addVertex(Vector2(-0.5f, 0.5f));
	poly->addVertex(Vector2(0.5f, 0.5f));
	poly->addVertex(Vector2(0.5f, -0.5f));	
}

Polygon* Polygon::getCircle()
{

}


}

