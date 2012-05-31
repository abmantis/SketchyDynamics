#ifndef PhySketchPolygon_h__
#define PhySketchPolygon_h__

#include "PhySketchDefinitions.h"
#include "PhySketchVector2.h"

namespace PhySketch
{
	class Polygon 
	{
		friend class Renderer;

	public:
		enum CoordinateSystem
		{
			CS_Scene	= 1,	// Scene space (the same used in ortho2d)
			CS_Pixel	= 2,	// Pixel space (window space)
			CS_Percent	= 3		// Percentage in window space
		};

	public:
		Polygon(CoordinateSystem cs = CS_Scene);
		virtual ~Polygon();

		/// <summary> Optimize vertex list, by removing duplicated and unreferenced vertices. </summary>
		//virtual void optimizeVertexList(); // TODO
		

		/// <summary> Gets the rotation angle of the polygon in degrees. </summary>
		/// 
		/// <returns> The angle in degrees. </returns>
		double getAngle(void) const;

		/// <summary> Sets the rotation angle of the polygon in degrees. </summary>
		///
		/// <param name="angle"> The new angle in degrees. </param>
		void setAngle(double angle);

		/// <summary> Gets the position of the polygon. </summary>
		///
		/// <returns> The position of the polygon. </returns>
		const Vector2& getPosition(void) const;

		/// <summary> Sets the position of the polygon. </summary>
		///
		/// <param name="position"> The new position of the polygon. </param>
		void setPosition(const Vector2& position);

		/// <summary> Gets the scale. </summary>
		///
		/// <returns> The scale. </returns>
		const Vector2& getScale(void) const;

		/// <summary> Sets the new scale of the polygon. </summary>
		///
		/// <param name="scale"> The new scale. </param>
		void setScale(const Vector2& scale);

		/// <summary> Translates the polygon </summary>
		///
		/// <param name="amount"> The amount to translate the polygon. </param>
		void translate(const Vector2& amount);

		/// <summary> Adds a new vertex . </summary>
		/// <param name="vertex"> The vertex. </param>
		void addVertex(const Vector2& vertex);

		/// <summary> Gets the coordinate system used by vertices. </summary>
		/// <returns> The coordinate system. </returns>
		const CoordinateSystem& getCoordinateSystem() const;

		/// <summary> Creates a unity sized square. </summary>
		/// <returns> The square. </returns>
		static Polygon* getSquare();
		
		/// <summary> Creates a unity sized circle (radius of 0.5). </summary>
		/// <param name="segments"> The number of segments that compose the circle. </param>
		/// <returns> The circle. </returns>
		static Polygon* getCircle(int num_segments);

	protected:
		std::vector<Vector2> _vertices;
		std::vector<int> _vertexIndexes;
		//std::vector<std::pair<Material, int> > _materials; // TODO later :) the int thing is to define to wich vertexIndex the material applies
		
		double _angle;
		Vector2 _position;
		Vector2 _scale;
		CoordinateSystem _coordSystem;	// the type of coordinates in wich vertices are defined
		
	};
}
#endif // PhySketchPolygon_h__