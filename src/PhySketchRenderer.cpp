#include "PhySketchRenderer.h"
#include "PhySketchPolygon.h"

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
		PHYSKETCH_ASSERT(ms_Singleton != NULL);
		return ms_Singleton;
	}

	Renderer& Renderer::getSingleton(void)
	{
		PHYSKETCH_ASSERT(ms_Singleton != NULL);
		return *ms_Singleton;
	}

	bool Renderer::initGL()
	{
		// Initialise GLEW
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::string glewErrorStr = "GLEW Error: ";
			glewErrorStr = glewErrorStr + (char*)glewGetErrorString(err);
			PHYSKETCH_LOG_ERROR(glewErrorStr);
			return FALSE;
		}

		// Check for OpenGL 2.0 support
		if (!GLEW_VERSION_2_0) {
			PHYSKETCH_LOG_ERROR("OpenGL 2.0 not available");
			return FALSE;
		}


		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
		//glClearDepth(1.0f);									// Depth Buffer Setup
		//glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		//glDepthFunc(GL_LEQUAL);							// The Type Of Depth Testing To Do
		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing because we are in 2D	
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		glDisable(GL_CULL_FACE);


		return true;		
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
		// change the view to pixel coordinates
		glMatrixMode(GL_PROJECTION);						
		glLoadIdentity();									
		gluOrtho2D(0, _viewportSize.x, _viewportSize.y, 0);
		glMatrixMode(GL_MODELVIEW);	
		glLoadIdentity();			

		polygon_set_iterator it = _pixelPolygons.begin();
		polygon_set_iterator it_end = _pixelPolygons.end();
		for(; it != _pixelPolygons.end(); it++)
		{
			renderPolygon(*it);	
		}
	}

	void Renderer::renderPercentPolygons() const
	{
		// change the view to percent coordinates
		glMatrixMode(GL_PROJECTION);						
		glLoadIdentity();									
		gluOrtho2D(0, 100, 100, 0);
		glMatrixMode(GL_MODELVIEW);	
		glLoadIdentity();

		polygon_set_iterator it = _percentPolygons.begin();
		polygon_set_iterator it_end = _percentPolygons.end();
		for(; it != _percentPolygons.end(); it++)
		{
			renderPolygon(*it);	
		}
	}

	void Renderer::renderScenePolygons() const
	{
		// change the view to scene coordinates
		glMatrixMode(GL_PROJECTION);						
		glLoadIdentity();									
		gluOrtho2D(_sceneViewMin.x, _sceneViewMax.x, 
			_sceneViewMin.y, _sceneViewMax.y);
		glMatrixMode(GL_MODELVIEW);	
		glLoadIdentity();


		polygon_set_iterator it = _scenePolygons.begin();
		polygon_set_iterator it_end = _scenePolygons.end();
		for(; it != _scenePolygons.end(); it++)
		{
			renderPolygon(*it);			
		}
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
			PHYSKETCH_ASSERT(poly->_vertexIndexes[j] < polygonVertexCount && "Vertex index out of range")

			vec = &poly->_vertices[poly->_vertexIndexes[j]];
			glVertex2d(vec->x, vec->y);
		}

		glEnd();
		glPopMatrix();	// pop individual polygon matrix
	}

	PhySketch::Vector2 Renderer::windowToScene( const Vector2 &vec )
	{
		PHYSKETCH_ASSERT(_viewportSize > Vector2(0,0));
			
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLdouble winX, winY, winZ;
		GLdouble posX, posY, posZ;

		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );

		winX = vec.x;
		winY = _windowSize.y - vec.y;
		glReadPixels( (int)vec.x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

		gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

		return Vector2(posX, posY);
	}
}

