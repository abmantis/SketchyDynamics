#include "PhySketchRenderer.h"
#include "PhySketchPolygon.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "PhySketchLogger.h"

namespace PhySketch
{
	template<> Renderer* Singleton<Renderer>::ms_Singleton = 0;

	Renderer::Renderer() : _viewAxisLimits(8.0f, 4.5f)
	{

	}

	Renderer::~Renderer()
	{
	}

	Renderer* Renderer::getSingletonPtr(void)
	{
		assert(ms_Singleton != NULL);
		return ms_Singleton;
	}

	Renderer& Renderer::getSingleton(void)
	{
		assert(ms_Singleton != NULL);
		return *ms_Singleton;
	}

	void Renderer::render() const
	{
		Polygon *pPolygon = nullptr;
		int polygonIndexCount = 0;
		int polygonVertexCount = 0;
		Vector2 *vec = nullptr;
		
		polygon_set_iterator it = _polygons.begin();
		polygon_set_iterator it_end = _polygons.end();
		for(; it != _polygons.end(); it++)
		{
			pPolygon = *it;
			glPushMatrix();
			glTranslated(pPolygon->_position.x, pPolygon->_position.y, 0.0f);
			glRotated(pPolygon->_angle, 0, 0, 1);
			glScaled(pPolygon->_scale.x, pPolygon->_scale.y, 1.0f);
			glBegin(GL_LINE_STRIP);
			
			polygonIndexCount = pPolygon->_vertexIndexes.size();
			polygonVertexCount = pPolygon->_vertices.size();
			for (int j = 0; j < polygonIndexCount; j++)
			{
				if(pPolygon->_vertexIndexes[j] >= polygonVertexCount)
				{
					Logger::getSingletonPtr()->writeError("Renderer::render",
						"Vertex index out of range");
				}
				
				vec = &pPolygon->_vertices[pPolygon->_vertexIndexes[j]];
				glVertex2d(vec->x, vec->y);
			}

			glEnd();
			glPopMatrix();
		}
	}

	void Renderer::addPolygon( Polygon *polygon )
	{
		std::pair<polygon_set_iterator, bool> res = _polygons.insert(polygon);
		if(res.second == false)
		{
			Logger::getSingletonPtr()->writeWarning("Renderer::addPolygon",
				"Tried do add a polygon that was already in the rendering list");
		}
	}

	void Renderer::removePolygon( Polygon *polygon )
	{
		_polygons.erase(polygon);
	}

	Vector2 Renderer::getViewAxisLimits() const
	{
		return _viewAxisLimits;
	}

}

