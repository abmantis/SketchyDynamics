#include "PhySketchRenderer.h"
#include "PhySketchPolygon.h"

#include "PhySketchLogger.h"
#include "PhySketchShader_GL.h"

namespace PhySketch
{
	template<> Renderer* Singleton<Renderer>::ms_Singleton = 0;

	Renderer::Renderer() : _sceneViewMin(-8.0f, -4.5f), _sceneViewMax(8.0f, 4.5f)
	{		
		_mainVertexShader	= nullptr;
		_mainFragmentShader	= nullptr;
		_mainShaderProgram	= nullptr;

		_shaderVars.attributes.position		= 0;
		_shaderVars.uniforms.transformation	= 0;
	}

	Renderer::~Renderer()
	{
		if(_mainShaderProgram)
		{
			delete _mainShaderProgram;
			_mainShaderProgram = nullptr;
		}

		if(_mainVertexShader)
		{
			delete _mainVertexShader;
			_mainVertexShader = nullptr;
		}

		if(_mainFragmentShader)
		{
			delete _mainFragmentShader;
			_mainFragmentShader = nullptr;
		}
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
		//glEnable(GL_LINE_SMOOTH);
		glLineWidth(2.0f);



		_mainVertexShader = new Shader_GL("../../../shaders/basic.v.glsl", GL_VERTEX_SHADER);
		if(!_mainVertexShader->isLoaded())
		{
			PHYSKETCH_LOG_ERROR("_mainVertexShader not loaded");
			return false;
		}
		
		_mainFragmentShader = new Shader_GL("../../../shaders/basic.f.glsl", GL_FRAGMENT_SHADER);
		if(!_mainFragmentShader->isLoaded())
		{
			PHYSKETCH_LOG_ERROR("_mainFragmentShader not loaded");
			return false;
		}

		_mainShaderProgram = new ShaderProgram_GL();
		_mainShaderProgram->addShaderToProgram(_mainVertexShader);
		_mainShaderProgram->addShaderToProgram(_mainFragmentShader);
		_mainShaderProgram->linkProgram();
		
		if (!_mainShaderProgram->isLinked())
		{
			PHYSKETCH_LOG_ERROR("_mainShaderProgram not linked");
			return false;
		}

		_shaderVars.attributes.position	
			= glGetAttribLocation(_mainShaderProgram->getProgramID(), "position");
		_shaderVars.uniforms.transformation	
			= glGetUniformLocation(_mainShaderProgram->getProgramID(), "trans_mat");
		_shaderVars.uniforms.color
			= glGetUniformLocation(_mainShaderProgram->getProgramID(), "color");

		return true;		
	}

	void Renderer::render() const
	{
		_mainShaderProgram->useProgram();

 		renderPixelPolygons();
 		renderPercentPolygons();
		renderScenePolygons();

		glUseProgram(NULL);
		//glUniformMatrix3fv(_shaderVars.uniforms.transformation, 1, GL_TRUE, Matrix3::IDENTITY[0]);
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
				

		glGenBuffers(1, &polygon->_vertexBuffer);
		glGenBuffers(1, &polygon->_elementBuffer);
		updateOpenGLBuffers(polygon);
		polygon->_hasNewVertices = false;
	}

	void Renderer::updateOpenGLBuffers( Polygon *polygon ) const
	{
		GLenum usageHint;
		switch(polygon->_vertexVariance)
		{
		case Polygon::VV_Dynamic:
			usageHint = GL_DYNAMIC_DRAW;
			break;
		case Polygon::VV_Stream:
			usageHint = GL_STREAM_DRAW;
			break;
		default:
			usageHint = GL_STATIC_DRAW;
			break;
		}

		float *vertBuff = new float[polygon->_vertices.size()*2];
		for (uint i = 0, j = 0; i < polygon->_vertices.size(); i++, j = j+2)
		{
			vertBuff[j] = polygon->_vertices[i].x;
			vertBuff[j+1] = polygon->_vertices[i].y;
		}

		uint *elemBuff = new uint[polygon->_vertexIndexes.size()];
		for (uint i = 0; i < polygon->_vertexIndexes.size(); i++)
		{
			elemBuff[i] = polygon->_vertexIndexes[i];
		}

		// Create vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, polygon->_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, polygon->_vertices.size()*sizeof(float)*2, 
			vertBuff, usageHint);

		// Create element buffers
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygon->_elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, polygon->_vertexIndexes.size()*sizeof(uint), 
			elemBuff, usageHint);

		//TODO: cleaning
		delete[] vertBuff;
		delete[] elemBuff;
	}

	void Renderer::removePolygon( Polygon *polygon )
	{
		glDeleteBuffers(1, &polygon->_vertexBuffer);
		glDeleteBuffers(1, &polygon->_elementBuffer);

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
		if(poly->_hasNewVertices)		
		{
			updateOpenGLBuffers(poly);
		}

		uint polygonIndexCount = poly->_vertexIndexes.size();
		Color color = poly->_material.getColor();
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
				

		glUniformMatrix3fv(_shaderVars.uniforms.transformation, 1, GL_TRUE,
			poly->_transformMatrix[0]);
		glUniform4f(_shaderVars.uniforms.color, color.r, color.g, color.b, 
			color.a);

		glBindBuffer(GL_ARRAY_BUFFER, poly->_vertexBuffer);
		glVertexAttribPointer(_shaderVars.attributes.position, 2, GL_FLOAT, 
			GL_FALSE, sizeof(GLfloat)*2, (void*)0);
		glEnableVertexAttribArray(_shaderVars.attributes.position);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, poly->_elementBuffer);
		glDrawElements(mode, polygonIndexCount, GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(_shaderVars.attributes.position);
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

		return Vector2(static_cast<float>(posX), static_cast<float>(posY));
	}

}

