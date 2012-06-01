#include "PhySketchPolygon.h"

namespace PhySketch
{

Polygon::Polygon(DrawingMode dm /*= DM_LINES*/, CoordinateSystem cs /*= CS_Scene*/) 
	: _angle(0), _position(0, 0), _scale(1, 1), _coordSystem(cs), _drawingMode(dm)
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

	_aabb.update(vertex);
}

const Polygon::CoordinateSystem& Polygon::getCoordinateSystem() const
{
	return _coordSystem;
}

Polygon* Polygon::CreateSquare()
{
	Polygon *poly = new Polygon(DM_LINE_LOOP, CS_Scene);
	poly->addVertex(Vector2(-0.5f, -0.5f));
	poly->addVertex(Vector2(-0.5f, 0.5f));
	poly->addVertex(Vector2(0.5f, 0.5f));
	poly->addVertex(Vector2(0.5f, -0.5f));	

	return poly;
}

Polygon* Polygon::CreateCircle( int num_segments )
{
	Polygon *poly = new Polygon(DM_LINE_LOOP, CS_Scene);

	//////////////////////////////////////////////////////////////////////////
	// Code from http://slabode.exofire.net/circle_draw.shtml
	double r = 0.5f;	// radius
	double theta = 2.0 * M_PI / double(num_segments); 
	double c = cos(theta);//precalculate the sine and cosine
	double s = sin(theta);
	double t;

	double x = r;//we start at angle = 0 
	double y = 0; 

	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		poly->addVertex(Vector2(x, y));

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	} 

	return poly;
}

const Polygon::DrawingMode& Polygon::getDrawingMode() const
{
	return(_drawingMode);
}

void Polygon::updateAABB()
{
	_aabb.invalidate();

	int nr_vertices = _vertices.size();
	for (int i = 0; i < nr_vertices; i++)
	{
		_aabb.update(_vertices[i]);
	}
}

const AABB& Polygon::getAABB() const
{
	return _aabb;
}


}

