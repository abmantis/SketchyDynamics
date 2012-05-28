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
		renderPixelPolygons();
		renderPercentPolygons();
		renderScenePolygons();
	}

	void Renderer::addPolygon( Polygon *polygon )
	{
		std::pair<polygon_set_iterator, bool> res;
		
		switch(polygon->_coordSystem)
		{
		case Polygon::CS_Pixel:
			res = _pixelPolygons.insert(polygon);
			break;
		case Polygon::CS_Percent:
			res = _percentPolygons.insert(polygon);
			break;
		case Polygon::CS_Scene:
			res = _scenePolygons.insert(polygon);
			break;
		default:
			return;
			break;
		}

		if(res.second == false)
		{
			Logger::getSingletonPtr()->writeWarning("Renderer::addPolygon",
				"Tried do add a polygon that was already in the rendering list");
		}
	}

	void Renderer::removePolygon( Polygon *polygon )
	{
		if(_pixelPolygons.erase(polygon) == 0)
		{
			if(_percentPolygons.erase(polygon) == 0)
			{
				_scenePolygons.erase(polygon);
			}
		}
	}

	Vector2 Renderer::getViewAxisLimits() const
	{
		return _viewAxisLimits;
	}

	void Renderer::renderPixelPolygons() const
	{
		Polygon *pPolygon = nullptr;
		int polygonIndexCount = 0;
		int polygonVertexCount = 0;
		Vector2 *vec = nullptr;

		polygon_set_iterator it = _pixelPolygons.begin();
		polygon_set_iterator it_end = _pixelPolygons.end();
		for(; it != _pixelPolygons.end(); it++)
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
					Logger::getSingletonPtr()->writeError("Renderer::renderPixelPolygons",
						"Vertex index out of range");
				}

				vec = &pPolygon->_vertices[pPolygon->_vertexIndexes[j]];
				glVertex2d(vec->x, vec->y);
			}

			glEnd();
			glPopMatrix();
		}

	}

	void Renderer::renderPercentPolygons() const
	{
		// TODO: scale+translate from percent to pixel
		Polygon *pPolygon = nullptr;
		int polygonIndexCount = 0;
		int polygonVertexCount = 0;
		Vector2 *vec = nullptr;

		polygon_set_iterator it = _percentPolygons.begin();
		polygon_set_iterator it_end = _percentPolygons.end();
		for(; it != _percentPolygons.end(); it++)
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

	void Renderer::renderScenePolygons() const
	{
		// TODO: scale+translate from scene to pixel
		Polygon *pPolygon = nullptr;
		int polygonIndexCount = 0;
		int polygonVertexCount = 0;
		Vector2 *vec = nullptr;

		polygon_set_iterator it = _scenePolygons.begin();
		polygon_set_iterator it_end = _scenePolygons.end();
		for(; it != _scenePolygons.end(); it++)
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

}

