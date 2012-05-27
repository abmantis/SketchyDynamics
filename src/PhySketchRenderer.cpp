#include "PhySketchRenderer.h"
#include "PhySketchMesh.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "PhySketchLogger.h"

namespace PhySketch
{
	template<> Renderer* Singleton<Renderer>::ms_Singleton = 0;

	Renderer::Renderer()
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
		Mesh *pMesh = nullptr;
		int meshIndexCount = 0;
		int meshVertexCount = 0;
		Vector2 *vec = nullptr;
		
		mesh_set_iterator it = _meshes.begin();
		mesh_set_iterator it_end = _meshes.end();
		for(; it != _meshes.end(); it++)
		{
			pMesh = *it;
			glPushMatrix();
			glTranslated(pMesh->_position.x, pMesh->_position.y, 0.0f);
			glRotated(pMesh->_angle, 0, 0, 1);
			glScaled(pMesh->_scale.x, pMesh->_scale.y, 1.0f);
			glBegin(GL_LINE_STRIP);
			
			meshIndexCount = pMesh->_vertexIndexes.size();
			meshVertexCount = pMesh->_vertices.size();
			for (int j = 0; j < meshIndexCount; j++)
			{
				if(pMesh->_vertexIndexes[j] >= meshVertexCount)
				{
					Logger::getSingletonPtr()->writeError("Renderer::render",
						"Vertex index out of range");
				}
				
				vec = &pMesh->_vertices[pMesh->_vertexIndexes[j]];
				glVertex2d(vec->x, vec->y);
			}

			glEnd();
			glPopMatrix();
		}
	}

	void Renderer::addMesh( Mesh *mesh )
	{
		std::pair<mesh_set_iterator, bool> res = _meshes.insert(mesh);
		if(res.second == false)
		{
			Logger::getSingletonPtr()->writeWarning("Renderer::addMesh",
				"Tried do add a mesh that was already in the rendering list");
		}
	}

	void Renderer::removeMesh( Mesh *mesh )
	{
		_meshes.erase(mesh);
	}

}

