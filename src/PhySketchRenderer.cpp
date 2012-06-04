#include "PhySketchRenderer.h"
#include "PhySketchPolygon.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "PhySketchLogger.h"

namespace PhySketch
{
	template<> Renderer* Singleton<Renderer>::ms_Singleton = 0;

	Renderer::Renderer() : _sceneViewMin(-8.0f, -4.5f), _sceneViewMax(8.0f, 4.5f)
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

	Vector2 Renderer::getSceneViewAxisMin() const
	{
		return _sceneViewMin;
	}


	Vector2 Renderer::getSceneViewAxisMax() const
	{
		return _sceneViewMax;
	}


	void Renderer::renderPixelPolygons() const
	{
		polygon_set_iterator it = _pixelPolygons.begin();
		polygon_set_iterator it_end = _pixelPolygons.end();
		for(; it != _pixelPolygons.end(); it++)
		{
			renderPolygon(*it);	
		}
	}

	void Renderer::renderPercentPolygons() const
	{
		Vector2 percentToPixelScale = _windowSize / 100.0f;

		// Transform from percentage coordinates to pixel coordinates
		glPushMatrix();	// push matrix for percent-to-pixel transformations
		glScaled(percentToPixelScale.x, percentToPixelScale.y, 0);

		polygon_set_iterator it = _percentPolygons.begin();
		polygon_set_iterator it_end = _percentPolygons.end();
		for(; it != _percentPolygons.end(); it++)
		{
			renderPolygon(*it);	
		}
		glPopMatrix(); // pop matrix for percent-to-pixel transformations
	}

	void Renderer::renderScenePolygons() const
	{
		Vector2 sceneToPixelScale = _windowSize / (_sceneViewMax - _sceneViewMin);
		Vector2 halfWindowSize = _windowSize / 2.0f;
		
		// Transform from scene coordinates to pixel coordinates
		glPushMatrix();	// push matrix for scene-to-pixel transformations
		glTranslated(halfWindowSize.x, halfWindowSize.y, 0);
		glScaled(sceneToPixelScale.x, -sceneToPixelScale.y, 0);		

		polygon_set_iterator it = _scenePolygons.begin();
		polygon_set_iterator it_end = _scenePolygons.end();
		for(; it != _scenePolygons.end(); it++)
		{
			renderPolygon(*it);			
		}
		glPopMatrix(); // pop matrix for scene-to-pixel transformations
	}

	void Renderer::renderPolygon( Polygon *poly ) const
	{

		// make sure the polygon is updated before being rendered
		poly->update();

		int polygonIndexCount = 0;
		int polygonVertexCount = 0;
		const Vector2 *vec = nullptr; 
		GLenum mode;
		switch(poly->_drawingMode)
		{
		case Polygon::DM_POINTS:
			mode = GL_POINTS;
			break;
		case Polygon::DM_LINES:
			mode = GL_LINES;
			break;
		case Polygon::DM_LINE_STRIP:
			mode = GL_LINE_STRIP;
			break;
		case Polygon::DM_LINE_LOOP:
			mode = GL_LINE_LOOP;
			break;
		case Polygon::DM_TRIANGLES:
			mode = GL_TRIANGLES;
			break;
		case Polygon::DM_TRIANGLE_STRIP:
			mode = GL_TRIANGLE_STRIP;
			break;
		case Polygon::DM_TRIANGLE_FAN:
			mode = GL_TRIANGLE_FAN;
			break;
		default:
			mode = GL_POINTS;
		}

		glPushMatrix();		// push matrix for this individual polygon
		glTranslated(poly->_position.x, poly->_position.y, 0.0f);
		glRotated(poly->_angle, 0, 0, 1);
		glScaled(poly->_scale.x, poly->_scale.y, 1.0f);
		glBegin(mode);

		polygonIndexCount = poly->_vertexIndexes.size();
		polygonVertexCount = poly->_vertices.size();
		for (int j = 0; j < polygonIndexCount; j++)
		{
			if(poly->_vertexIndexes[j] >= polygonVertexCount)
			{
				Logger::getSingletonPtr()->writeError("Renderer::renderPolygon",
					"Vertex index out of range");
			}

			vec = &poly->_vertices[poly->_vertexIndexes[j]];
			glVertex2d(vec->x, vec->y);
		}

		glEnd();
		glPopMatrix();	// pop individual polygon matrix
	}

}

