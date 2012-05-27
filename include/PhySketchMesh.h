#ifndef PhySketchMesh_h__
#define PhySketchMesh_h__

#include "PhySketchDefinitions.h"
#include "PhySketchVector2.h"

namespace PhySketch
{
	class Mesh 
	{
		friend class Renderer;
	public:
		Mesh();
		virtual ~Mesh();

		/// <summary> Optimize vertex list, by removing duplicated and unreferenced vertices. </summary>
		//virtual void optimizeVertexList(); // TODO
		

		/// <summary> Gets the rotation angle of the mesh in degrees. </summary>
		/// 
		/// <returns> The angle in degrees. </returns>
		double getAngle(void) const;

		/// <summary> Sets the rotation angle of the mesh in degrees. </summary>
		///
		/// <param name="angle"> The new angle in degrees. </param>
		void setAngle(double angle);

		/// <summary> Gets the position of the mesh. </summary>
		///
		/// <returns> The position of the mesh. </returns>
		const Vector2& getPosition(void) const;

		/// <summary> Sets the position of the mesh. </summary>
		///
		/// <param name="position"> The new position of the mesh. </param>
		void setPosition(const Vector2& position);

		/// <summary> Gets the scale. </summary>
		///
		/// <returns> The scale. </returns>
		const Vector2& getScale(void) const;

		/// <summary> Sets the new scale of the mesh. </summary>
		///
		/// <param name="scale"> The new scale. </param>
		void setScale(const Vector2& scale);

		/// <summary> Translates the mesh </summary>
		///
		/// <param name="amount"> The amount to translate the mesh. </param>
		void translate(const Vector2& amount);

		/// <summary> Adds a new vertex . </summary>
		/// <param name="vertex"> The vertex. </param>
		void addVertex(const Vector2& vertex);

	protected:
		std::vector<Vector2> _vertices;
		std::vector<int> _vertexIndexes;
		//std::vector<std::pair<Material, int> > _materials; // TODO later :) the int thing is to define to wich vertexIndex the material applies
		
		double _angle;
		Vector2 _position;
		Vector2 _scale;
		
	};
}
#endif // PhySketchMesh_h__