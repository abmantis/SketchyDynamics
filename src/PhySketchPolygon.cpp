#include "PhySketchPolygon.h"
#include "PhySketchUtils.h"

namespace PhySketch
{

//////////////////////////////////////////////////////////////////////////
// SubPolygon class

SubPolygon::SubPolygon( DrawingMode dm ) :
	_drawingMode	(dm),
	_hasNewVertices	(false), 
	_vertexBuffer	(0), 
	_elementBuffer	(0)
{
}

void SubPolygon::addVertex( const Vector2& vertex )
{
	_vertexIndexes.push_back(_vertexIndexes.size());
	_vertices.push_back(vertex);	

	_aabb.update(vertex);

	_hasNewVertices = true;
}

const Material& SubPolygon::GetMaterial( void ) const
{
	return(_material);
}

void SubPolygon::SetMaterial( const Material& material )
{
	_material = material;
}

const DrawingMode& SubPolygon::getDrawingMode() const
{
	return(_drawingMode);
}

bool SubPolygon::isPointInside( const Vector2& pt ) const
{
	uint vertCount = _vertexIndexes.size();
	Vector2 p1, p2, p3; // triangle points

	if(vertCount <= 0)
		return false;

	if( _drawingMode == DM_TRIANGLES)
	{
		uint i = 0; 
		while (i+2 < vertCount)
		{			
			p1 = _vertices[_vertexIndexes[i]];
			p2 = _vertices[_vertexIndexes[++i]];
			p3 = _vertices[_vertexIndexes[++i]];
			++i;

			if(isPointInTri(pt, p1, p2, p3) == true)
			{
				return true;
			}
		}
	}
	else if(_drawingMode == DM_TRIANGLE_FAN)
	{
		uint i = 1; 
		p1 = _vertices[_vertexIndexes[0]];

		while (i+1 < vertCount)
		{			
			p2 = _vertices[_vertexIndexes[i]];
			p3 = _vertices[_vertexIndexes[++i]];


			if(isPointInTri(pt, p1, p2, p3) == true)
			{
				return true;
			}
		}
	}
	else if(_drawingMode == DM_TRIANGLE_STRIP)
	{
		uint i = 1; 
		while (i+1 < vertCount)
		{			
			p1 = _vertices[_vertexIndexes[i-1]];
			p2 = _vertices[_vertexIndexes[i]];
			p3 = _vertices[_vertexIndexes[++i]];


			if(isPointInTri(pt, p1, p2, p3) == true)
			{
				return true;
			}
		}
	}

	return false;
}

void SubPolygon::updateAABB()
{
	_aabb.invalidate();

	int nr_vertices = _vertices.size();
	for (int i = 0; i < nr_vertices; i++)
	{
		_aabb.update(_vertices[i]);
	}
}

const AABB& SubPolygon::getAABB() const
{
	return _aabb;
}

PhySketch::AABB SubPolygon::getWorldAABB( bool bestFit, Matrix3 transformMatrix ) const
{
	AABB aabb;
	if(bestFit)
	{
		// transform every point and compute the AABB
		int nr_vertices = _vertices.size();
		for (int i = 0; i < nr_vertices; i++)
		{			
			aabb.update(transformMatrix * _vertices[i]);
		}		
	}
	else
	{
		// only transform the AABB
		aabb = _aabb;
		aabb.transform(transformMatrix);
	}
	return aabb;
}


//////////////////////////////////////////////////////////////////////////
// Polygon class

Polygon::Polygon(VertexVariance vv /*= VV_Static*/, std::string name /*= ""*/, CoordinateSystem cs /*= CS_Scene*/) :
	_name(name),	
	_angle			(0.0f), 
	_position		(Vector2::ZERO_XY), 
	_scale			(Vector2::UNIT_XY), 
	_transformMatrix(Matrix3::IDENTITY),
	_coordSystem	(cs), 
	_vertexVariance	(vv),
	_inRenderingQueue(false),
	_userData(NULL),
	_userType(PHYSKETCH_POLYGON_UTYPE_NONE)
{
}

Polygon::~Polygon()
{
	clearSubPolygons();
}

void Polygon::clearSubPolygons()
{
	uint subpolycount = _subPolygons.size();
	for (uint i = 0; i < subpolycount; ++i)
	{
		delete _subPolygons[i];
	}
	_subPolygons.clear();
}

float Polygon::getAngle( void ) const
{
	return(_angle);
}

void Polygon::setAngle( float angle )
{
	if (angle != _angle)
	{
		_angle = angle;
		computeTransformationMatrix();
	}	
}

const Vector2& Polygon::getPosition( void ) const
{
	return(_position);
}

void Polygon::setPosition( const Vector2& position )
{
	if(position != _position)
	{
		_position = position;
		computeTransformationMatrix();
	}
}

const Vector2& Polygon::getScale( void ) const
{
	return(_scale);
}

void Polygon::setScale( const Vector2& scale )
{
	if (scale != _scale)
	{
		_scale = scale;
		computeTransformationMatrix();
	}	
}

void Polygon::translate( const Vector2& amount )
{
	_position += amount;
	computeTransformationMatrix();
}

void Polygon::rotate( const float& angle )
{
	_angle += angle;
	computeTransformationMatrix();
}

void Polygon::scale( const Vector2& factor )
{
	_scale *= factor;
	computeTransformationMatrix();
}

const CoordinateSystem& Polygon::getCoordinateSystem() const
{
	return _coordSystem;
}

SubPolygon* Polygon::CreateSquareSubPolygon( DrawingMode dm )
{
	SubPolygon *subpoly = createSubPolygon(dm);

	subpoly->addVertex(Vector2(-0.5f, -0.5f));
	subpoly->addVertex(Vector2(-0.5f, 0.5f));
	subpoly->addVertex(Vector2(0.5f, 0.5f));
	subpoly->addVertex(Vector2(0.5f, -0.5f));	

	return subpoly;
}

SubPolygon* Polygon::CreateCircleSubPolygon( DrawingMode dm, Vector2 center, float radius, int num_segments )
{
	float theta = 2.0f * M_PI / float(num_segments); 
	float c = cos(theta);//precalculate the sine and cosine
	float s = sin(theta);
	float t;

	float x = radius;//we start at angle = 0 
	float y = 0; 

	SubPolygon *subpoly = createSubPolygon(dm);
	subpoly->_vertexIndexes.reserve(num_segments);
	subpoly->_vertices.reserve(num_segments);

	for(int ii = 0; ii < num_segments; ++ii) 
	{ 
		subpoly->addVertex(Vector2(x + center.x, y + center.y));

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	} 

	return subpoly;
}

std::vector<Vector2> Polygon::GetCircleVertices( Vector2 center, float radius, int num_segments )
{
	//////////////////////////////////////////////////////////////////////////
	// Code from http://slabode.exofire.net/circle_draw.shtml
	std::vector<Vector2> circleVec;
	
	float theta = 2.0f * M_PI / float(num_segments); 
	float c = cos(theta);//precalculate the sine and cosine
	float s = sin(theta);
	float t;

	float x = radius;//we start at angle = 0 
	float y = 0; 

	circleVec.reserve(num_segments);
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		circleVec.push_back(Vector2(x + center.x, y + center.y));

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	} 

	return circleVec;
}

void Polygon::updateAABB()
{
	_aabb.invalidate();

	uint subpolycount = _subPolygons.size();
	for (uint i = 0; i < subpolycount; ++i)
	{
		_aabb.update(_subPolygons[i]->_aabb);
	}
}

const AABB& Polygon::getAABB() const
{
	return _aabb;
}

PhySketch::AABB Polygon::getWorldAABB( bool bestFit ) const
{
	AABB aabb;
	if(bestFit)
	{
		uint subpolycount = _subPolygons.size();
		for (uint i = 0; i < subpolycount; ++i)
		{
			aabb.update(_subPolygons[i]->getWorldAABB(bestFit, _transformMatrix));
		}
	}
	else
	{
		// only transform the AABB
		aabb = _aabb;
		aabb.transform(_transformMatrix);
	}
	return aabb;
}

const VertexVariance& Polygon::GetVertexVariance() const
{
	return(_vertexVariance);
}

void Polygon::computeTransformationMatrix()
{
	Matrix3 translation, rotation, scale;
	
	translation.fromPosition(_position);
	rotation.fromAngle(_angle);
	scale.fromScale(_scale);

	_transformMatrix = translation*rotation*scale;

}

std::string Polygon::getName() const
{
	return _name;
}

void Polygon::setUserType( ulong type )
{
	_userType = type;
}

PhySketch::ulong Polygon::getUserType() const
{
	return _userType;
}

void Polygon::setUserData( void* data )
{
	_userData = data;
}

void* Polygon::getUserData()
{
	return _userData;
}

bool Polygon::isPointInside( const Vector2& pt ) const
{
	uint subpolycount = _subPolygons.size();
	for (uint i = 0; i < subpolycount; ++i)
	{
		if(_subPolygons[i]->isPointInside(pt))
		{
			return true;
		}
	}

	return false;
}

SubPolygon* Polygon::createSubPolygon( DrawingMode dm )
{
	SubPolygon *subpoly = new SubPolygon(dm);
	_subPolygons.push_back(subpoly);
	return subpoly;
}

PhySketch::uint Polygon::getSubPolygonCount() const
{
	return _subPolygons.size();
}

SubPolygon* Polygon::getSubPolygon( uint i )
{
	return _subPolygons[i];
}


}

