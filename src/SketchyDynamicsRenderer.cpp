#include "SketchyDynamicsRenderer.h"
#include "SketchyDynamicsPolygon.h"

#include "SketchyDynamicsLogger.h"
#include "SketchyDynamicsShader_GL.h"

namespace SketchyDynamics
{
	template<> Renderer* Singleton<Renderer>::ms_Singleton = 0;

	Renderer::Renderer() : 
		_sceneViewMin		(-8.0f, -4.5f), 
		_sceneViewMax		(8.0f, 4.5f), 
		_mainVertexShader	(nullptr),
		_mainFragmentShader	(nullptr),
		_mainShaderProgram	(nullptr)		
	{		
		_shaderVars.attributes.position		= 0;
		_shaderVars.attributes.textCoord	= 0;
		_shaderVars.uniforms.transformation	= 0;
		_shaderVars.uniforms.color			= 0;
		_shaderVars.uniforms.textureFlag	= 0;
		_shaderVars.uniforms.texture		= 0;
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
		SKETCHYDYNAMICS_ASSERT(ms_Singleton != NULL);
		return ms_Singleton;
	}

	Renderer& Renderer::getSingleton(void)
	{
		SKETCHYDYNAMICS_ASSERT(ms_Singleton != NULL);
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
			SKETCHYDYNAMICS_LOG_ERROR(glewErrorStr);
			return FALSE;
		}

		// Check for OpenGL 2.0 support
		if (!GLEW_VERSION_2_0) {
			SKETCHYDYNAMICS_LOG_ERROR("OpenGL 2.0 not available");
			return FALSE;
		}


		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing because we are in 2D	
		glDisable(GL_CULL_FACE);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST );
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST );
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2.0f);
		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );



		_mainVertexShader = new Shader_GL("../../../shaders/basic.v.glsl", GL_VERTEX_SHADER);
		if(!_mainVertexShader->isLoaded())
		{
			SKETCHYDYNAMICS_LOG_ERROR("_mainVertexShader not loaded");
			return false;
		}
		
		_mainFragmentShader = new Shader_GL("../../../shaders/basic.f.glsl", GL_FRAGMENT_SHADER);
		if(!_mainFragmentShader->isLoaded())
		{
			SKETCHYDYNAMICS_LOG_ERROR("_mainFragmentShader not loaded");
			return false;
		}

		_mainShaderProgram = new ShaderProgram_GL();
		_mainShaderProgram->addShaderToProgram(_mainVertexShader);
		_mainShaderProgram->addShaderToProgram(_mainFragmentShader);
		_mainShaderProgram->linkProgram();
		
		if (!_mainShaderProgram->isLinked())
		{
			SKETCHYDYNAMICS_LOG_ERROR("_mainShaderProgram not linked");
			return false;
		}

		_shaderVars.attributes.position	
			= glGetAttribLocation(_mainShaderProgram->getProgramID(), "position");
		_shaderVars.attributes.textCoord
			= glGetAttribLocation(_mainShaderProgram->getProgramID(), "texCoord");
		_shaderVars.uniforms.transformation	
			= glGetUniformLocation(_mainShaderProgram->getProgramID(), "transMat");
		_shaderVars.uniforms.color
			= glGetUniformLocation(_mainShaderProgram->getProgramID(), "color");
		_shaderVars.uniforms.textureFlag
			= glGetUniformLocation(_mainShaderProgram->getProgramID(), "textureFlag");
		_shaderVars.uniforms.texture
			= glGetUniformLocation(_mainShaderProgram->getProgramID(), "texture");

		return true;		
	}

	void Renderer::render(ulong timeSinceLastFrame)
	{
		_mainShaderProgram->useProgram();

		renderPolygons(timeSinceLastFrame);

		glUseProgram(NULL);
		//glUniformMatrix3fv(_shaderVars.uniforms.transformation, 1, GL_TRUE, Matrix3::IDENTITY[0]);
	}

	void Renderer::addPolygon( Polygon *polygon, ulong depth, RenderQueueType rq /*= RQT_Scene*/ )
	{
		SKETCHYDYNAMICS_ASSERT(!polygon->_inRenderingQueue && "Polygon is already in the rendering queue");

		if(!polygon->getName().empty())
		{
			std::pair<NamedPolygonMap::iterator, bool> insresult = 
				_namedPolygonMap.insert(NamedPolygonMap::value_type(polygon->getName(), polygon));

			SKETCHYDYNAMICS_ASSERT(insresult.second && "A Polygon with the same name already exists in the renderer.");			
		}

		RenderQueue::iterator it_begin;
		RenderQueue::iterator it;
		RenderQueue *currentQueue = getRenderQueuePtr(rq);

		if(currentQueue == nullptr)
		{
			return;
		}
		
		it_begin = currentQueue->begin();
		it = currentQueue->end();

		for(; it != it_begin; /* we'll update the iterator inside the loop */)
		{
			it--;

			if(it->depth <= depth)
			{
				it++;	// advance one element because list::insert inserts *before* the iterator position
				break;
			}
		}	
		
		currentQueue->insert(it, RenderQueueParams(polygon, depth));

		SubPolygon *subpoly = nullptr;
		uint subpolycount = polygon->_subPolygons.size();
		for (uint i = 0; i < subpolycount; ++i)
		{
			updateOpenGLBuffers(polygon->_subPolygons[i], polygon->_vertexVariance);
		}
		
		polygon->_inRenderingQueue = true;
	}

	void Renderer::addPolygon( Polygon *polygon, RenderQueueType rq /*= RQT_Scene*/ )
	{
		RenderQueue *currentQueue = getRenderQueuePtr(rq);
		ulong depth = 0;

		if (!currentQueue->empty())
		{
			depth = currentQueue->back().depth+1;
		}

		addPolygon(polygon, depth, rq);
	}

	void Renderer::updateOpenGLBuffers( SubPolygon *subpolygon, VertexVariance variance ) const
	{
		GLenum usageHint;
		switch(variance)
		{
		case VV_Dynamic:
			usageHint = GL_DYNAMIC_DRAW;
			break;
		case VV_Stream:
			usageHint = GL_STREAM_DRAW;
			break;
		default:
			usageHint = GL_STATIC_DRAW;
			break;
		}

		if (subpolygon->_vertexBuffer == 0)
		{
			glGenBuffers(1, &subpolygon->_vertexBuffer);
		}
		if (subpolygon->_texCoordBuffer == 0)
		{
			glGenBuffers(1, &subpolygon->_texCoordBuffer);
		}
		if (subpolygon->_elementBuffer == 0)
		{
			glGenBuffers(1, &subpolygon->_elementBuffer);
		}
		

		float *vertBuff = new float[subpolygon->_vertices.size()*2];
		for (uint i = 0, j = 0; i < subpolygon->_vertices.size(); i++, j = j+2)
		{
			vertBuff[j] = subpolygon->_vertices[i].x;
			vertBuff[j+1] = subpolygon->_vertices[i].y;
		}

		float *texCoordBuff = new float[subpolygon->_textureCoords.size()*2];
		for (uint i = 0, j = 0; i < subpolygon->_textureCoords.size(); i++, j = j+2)
		{
			texCoordBuff[j] = subpolygon->_textureCoords[i].x;
			texCoordBuff[j+1] = subpolygon->_textureCoords[i].y;
		}


		uint *elemBuff = new uint[subpolygon->_vertexIndexes.size()];
		for (uint i = 0; i < subpolygon->_vertexIndexes.size(); i++)
		{
			elemBuff[i] = subpolygon->_vertexIndexes[i];
		}

		
		// Create vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, subpolygon->_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, subpolygon->_vertices.size()*sizeof(float)*2, 
			vertBuff, usageHint); 	// TODO: use glSubBufferData?

		// Create texture coordinate buffers
		glBindBuffer(GL_ARRAY_BUFFER, subpolygon->_texCoordBuffer);
		glBufferData(GL_ARRAY_BUFFER, subpolygon->_textureCoords.size()*sizeof(float)*2, 
			texCoordBuff, usageHint); 	// TODO: use glSubBufferData?

		// Create element buffers
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subpolygon->_elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, subpolygon->_vertexIndexes.size()*sizeof(uint), 
			elemBuff, usageHint); 	// TODO: use glSubBufferData?


		delete[] vertBuff;
		delete[] texCoordBuff;
		delete[] elemBuff;		

		subpolygon->_hasNewVertices = false;
	}

	void Renderer::removePolygon( Polygon *polygon, RenderQueueType rq /*= RQT_Scene*/ )
	{
		SubPolygon *subpoly = nullptr;
		uint subpolycount = polygon->_subPolygons.size();
		for (uint i = 0; i < subpolycount; ++i)
		{
			subpoly = polygon->_subPolygons[i];
			glDeleteBuffers(1, &subpoly->_vertexBuffer);
			glDeleteBuffers(1, &subpoly->_texCoordBuffer);
			glDeleteBuffers(1, &subpoly->_elementBuffer);

			subpoly->_vertexBuffer	= 0;
			subpoly->_texCoordBuffer = 0;
			subpoly->_elementBuffer	= 0;
		}

		RenderQueue *currentQueue = getRenderQueuePtr(rq);

		RenderQueue::iterator it = currentQueue->begin();
		RenderQueue::iterator it_end = currentQueue->end();
		for(; it != it_end; it++)
		{
			if(it->polygon == polygon)
			{
				currentQueue->erase(it);
				break;
			}
		}

		if(!polygon->getName().empty())
		{
			_namedPolygonMap.erase(polygon->getName());
		}

		polygon->_inRenderingQueue = false;
	}

	Vector2 Renderer::getSceneViewAxisMin() const
	{
		return _sceneViewMin;
	}
	
	Vector2 Renderer::getSceneViewAxisMax() const
	{
		return _sceneViewMax;
	}
	
	void Renderer::renderPolygons(ulong timeSinceLastFrame)
	{
		RenderQueue::iterator it, it_end;
		Polygon* poly = nullptr;

		it = _backgroundRenderQueue.begin();
		it_end = _backgroundRenderQueue.end();
		for(; it != it_end; it++)
		{
			poly = it->polygon;
			if(poly->_visible)
			{
				renderPolygon(poly, timeSinceLastFrame);
			}
		}

		it = _sceneRenderQueue.begin();
		it_end = _sceneRenderQueue.end();
		for(; it != it_end; it++)
		{
			poly = it->polygon;
			if(poly->_visible)
			{
				renderPolygon(poly, timeSinceLastFrame);
			}
		}

		it = _uiRenderQueue.begin();
		it_end = _uiRenderQueue.end();
		for(; it != it_end; it++)
		{
			poly = it->polygon;
			if(poly->_visible)
			{
				renderPolygon(poly, timeSinceLastFrame);
			}
		}
	}

	void Renderer::renderPolygon( Polygon *poly, ulong timeSinceLastFrame ) 
	{
		// make sure the polygon is updated before being rendered
		poly->update(timeSinceLastFrame);

		glUniformMatrix3fv(_shaderVars.uniforms.transformation, 1, GL_TRUE,
			poly->_transformMatrix[0]);		
		
		//////////////////////////////////////////////////////////////////////////
		// render each SubPolygon
		SubPolygon *subpoly = nullptr;
		uint subpolycount = poly->_subPolygons.size();
		for (uint i = 0; i < subpolycount; ++i)
		{		
			subpoly = poly->_subPolygons[i];
			if(!subpoly->_visible)
			{
				continue;
			}
			if(subpoly->_hasNewVertices)		
			{
				updateOpenGLBuffers(subpoly, poly->_vertexVariance);
				poly->updateAABB();
			}

			uint indexCount = subpoly->_vertexIndexes.size();
			Material *mat = subpoly->_material;
			Color color = mat->getColor();
			uint texID = mat->getTextureID();
			GLenum mode;
			switch(subpoly->_drawingMode)
			{
			case DM_POINTS:
				mode = GL_POINTS;
				break;
			case DM_LINES:
				mode = GL_LINES;
				break;
			case DM_LINE_STRIP:
				mode = GL_LINE_STRIP;
				break;
			case DM_LINE_LOOP:
				mode = GL_LINE_LOOP;
				break;
			case DM_TRIANGLES:
				mode = GL_TRIANGLES;
				break;
			case DM_TRIANGLE_STRIP:
				mode = GL_TRIANGLE_STRIP;
				break;
			case DM_TRIANGLE_FAN:
				mode = GL_TRIANGLE_FAN;
				break;
			default:
				mode = GL_POINTS;
			}	
	

			glUniform4f(_shaderVars.uniforms.color, color.r, color.g, color.b, 
				color.a*poly->_alpha);

			if(texID > 0)
			{
				//glActiveTexture(GL_TEXTURE0);  TODO: when we implement multi-texture materials, check what the current text unit is and only activate if it is different. 
				glBindTexture(GL_TEXTURE_2D, texID);
				glUniform1i(_shaderVars.uniforms.texture, 0);
				glUniform1f(_shaderVars.uniforms.textureFlag, 1.0f);
			}
			else
			{
				glUniform1f(_shaderVars.uniforms.textureFlag, 0.0f);
			}


			glBindBuffer(GL_ARRAY_BUFFER, subpoly->_vertexBuffer);
			glVertexAttribPointer(_shaderVars.attributes.position, 2, GL_FLOAT, 
				GL_FALSE, sizeof(GLfloat)*2, (void*)0);
			glEnableVertexAttribArray(_shaderVars.attributes.position);

			glBindBuffer(GL_ARRAY_BUFFER, subpoly->_texCoordBuffer);
			glVertexAttribPointer(_shaderVars.attributes.textCoord, 2, GL_FLOAT, 
				GL_FALSE, sizeof(GLfloat)*2, (void*)0);
			glEnableVertexAttribArray(_shaderVars.attributes.textCoord);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subpoly->_elementBuffer);
			glDrawElements(mode, indexCount, GL_UNSIGNED_INT, (void*)0);

			glDisableVertexAttribArray(_shaderVars.attributes.textCoord);
			glDisableVertexAttribArray(_shaderVars.attributes.position);
		}

	}
	

	SketchyDynamics::Vector2 Renderer::windowToScene( const Vector2 &vec )
	{
		SKETCHYDYNAMICS_ASSERT(_viewportSize > Vector2(0,0));
			
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

	Renderer::RenderQueue* Renderer::getRenderQueuePtr( RenderQueueType rqp )
	{
		switch(rqp)
		{
		case RQT_Background:
			return &_backgroundRenderQueue;
			break;
		case RQT_Scene:
			return &_sceneRenderQueue;
			break;
		case RQT_UI:
			return &_uiRenderQueue;
			break;
		default:
			SKETCHYDYNAMICS_LOG_ERROR("Invalid render queue");
			return nullptr;
		}
	}

	Polygon* Renderer::getPolygonByName( std::string name )
	{
		// Look up 
		NamedPolygonMap::iterator i = _namedPolygonMap.find(name);

		if (i == _namedPolygonMap.end())
		{
			return nullptr;
		}

		return i->second;
	}

	void Renderer::queryScene( const Vector2& pt, SceneQueryCallback *callback )
	{
		bool continueQuery = true;
		RenderQueue::iterator it, it_begin;
		Polygon *p = nullptr;
		AABB aabb;

		for (int i = 0; i < 3; ++i)
		{
			if(i == 0)
			{
				it_begin = _uiRenderQueue.begin();
				it = _uiRenderQueue.end();	
			}
			else if (i == 1)
			{
				it_begin = _sceneRenderQueue.begin();
				it = _sceneRenderQueue.end();
			} 
			else if (i == 2)
			{				
				it_begin = _backgroundRenderQueue.begin();
				it = _backgroundRenderQueue.end();
			}
			else
			{
				break;
			}

			for(; it != it_begin; /* we'll update the iterator inside the loop */)
			{
				it--;

				p = it->polygon;
				if(p->_visible)
				{
					aabb = p->getTransformedAABB(true);
					if(aabb.isPointInside(pt))
					{
						continueQuery = callback->reportPolygon(p);
						if(continueQuery == false)
						{
							break;
						}
					}	
				}		
			}


			if(continueQuery == false)
			{
				break;
			}
		}		
		
	}

}

