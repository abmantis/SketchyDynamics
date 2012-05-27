#include "PhySketchMesh.h"

namespace PhySketch
{

Mesh::Mesh() : _angle(0), _position(0, 0), _scale(1, 1)
{
}

Mesh::~Mesh()
{
}

double Mesh::getAngle( void ) const
{
	return(_angle);
}

void Mesh::setAngle( double angle )
{
	_angle = angle;
}

const Vector2& Mesh::getPosition( void ) const
{
	return(_position);
}

void Mesh::setPosition( const Vector2& position )
{
	_position = position;
}

const Vector2& Mesh::getScale( void ) const
{
	return(_scale);
}

void Mesh::setScale( const Vector2& scale )
{
	_scale = scale;
}

void Mesh::translate( const Vector2& amount )
{
	_position += amount;
}

void Mesh::addVertex( const Vector2& vertex )
{
	_vertexIndexes.push_back(_vertexIndexes.size());
	_vertices.push_back(vertex);	
}


}

