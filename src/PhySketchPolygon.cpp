#include "PhySketchPolygon.h"

namespace PhySketch
{

Polygon::Polygon() : _angle(0), _position(0, 0), _scale(1, 1)
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


}

