#ifndef PhySketchRenderer_h__
#define PhySketchRenderer_h__

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"

namespace PhySketch
{
	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer();
		virtual ~Renderer();

		/// <summary> Adds a mesh to the rendering list. </summary>
		/// <param name="mesh"> The new mesh. </param>
		virtual void addMesh(Mesh *mesh);

		/// <summary> Removes the mesh from the rendering list. </summary>
		/// <param name="mesh"> The mesh to remove. </param>
		/// <remarks> This function will not clear any of the meshe's resources </remakrs>
		virtual void removeMesh(Mesh *mesh);
	
		/// <summary> Renders the meshes in the mesh list to screen. </summary>		
		virtual void render() const;

		static Renderer* getSingletonPtr(void);
		static Renderer& getSingleton(void);

	protected:
		std::set<Mesh*> _meshes;


		typedef std::set<Mesh*>::iterator mesh_set_iterator;
		
	};
}
#endif // PhySketchRenderer_h__