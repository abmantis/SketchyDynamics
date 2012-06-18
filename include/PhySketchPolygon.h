#ifndef PhySketchPolygon_h__
#define PhySketchPolygon_h__

#include "PhySketchDefinitions.h"
#include "PhySketchVector2.h"
#include "PhySketchAABB.h"
#include "PhySketchMatrix3.h"


namespace PhySketch
{
	class Polygon 
	{
		friend class Renderer;

	public:
		enum CoordinateSystem
		{
			CS_Scene,		// Scene space (the same used in ortho2d)
			CS_Pixel,		// Pixel space (window space)
			CS_Percent		// Percentage in window space
		};

		enum DrawingMode
		{
			DM_POINTS,
			DM_LINES, 
			DM_LINE_STRIP, 
			DM_LINE_LOOP, 
			DM_TRIANGLES, 
			DM_TRIANGLE_STRIP, 
			DM_TRIANGLE_FAN
		};

		/// <summary> Values that represent how much the vertices vary. </summary>
		/// <remarks> The variance is only related to vertices and not Polygon's
		/// 	position, scale or angle. </remarks>
		enum VertexVariance
		{
			VV_Static,	// For polygons with static vertices
			VV_Dynamic,	// For polygons that change vertices frequently
			VV_Stream	// For polygons that change vertices almost every frame
		};
	public:
		Polygon(VertexVariance vv = VV_Static , DrawingMode dm = DM_LINES, CoordinateSystem cs = CS_Scene);
		virtual ~Polygon();

		/// <summary> Optimize vertex list, by removing duplicated and unreferenced vertices. </summary>
		//virtual void optimizeVertexList(); // TODO
		

		/// <summary> Gets the rotation angle of the polygon in degrees. </summary>
		/// 
		/// <returns> The angle in degrees. </returns>
		virtual float getAngle(void) const;

		/// <summary> Sets the rotation angle of the polygon in degrees. </summary>
		///
		/// <param name="angle"> The new angle in degrees. </param>
		virtual void setAngle(float angle);

		/// <summary> Gets the position of the polygon. </summary>
		///
		/// <returns> The position of the polygon. </returns>
		virtual const Vector2& getPosition(void) const;

		/// <summary> Sets the position of the polygon. </summary>
		///
		/// <param name="position"> The new position of the polygon. </param>
		virtual void setPosition(const Vector2& position);

		/// <summary> Gets the scale. </summary>
		///
		/// <returns> The scale. </returns>
		virtual const Vector2& getScale(void) const;

		/// <summary> Sets the new scale of the polygon. </summary>
		///
		/// <param name="scale"> The new scale. </param>
		virtual void setScale(const Vector2& scale);

		/// <summary> Translates the polygon </summary>
		///
		/// <param name="amount"> The amount to translate the polygon. </param>
		virtual void translate(const Vector2& amount);

		/// <summary> Adds a new vertex . </summary>
		/// <param name="vertex"> The vertex. </param>
		virtual void addVertex(const Vector2& vertex);

		/// <summary> Gets the coordinate system used by vertices. </summary>
		/// <returns> The coordinate system. </returns>
		virtual const CoordinateSystem& getCoordinateSystem() const;

		/// <summary> Gets the drawing mode. </summary>
		/// <returns> The drawing mode. </returns>
		virtual const DrawingMode& getDrawingMode() const;

		/// <summary> Sets the drawing mode. </summary>
		/// <param name="dm"> The DrawingMode. </param>
		virtual void setDrawingMode(DrawingMode dm);

		/// <summary> Updates the axis aligned bounding box. </summary>
		virtual void updateAABB();

		/// <summary> Gets the axis aligned bounding box of this mesh. </summary>
		/// <returns> The axis aligned bounding box. </returns>
		virtual const AABB& getAABB() const;

		
		/// <summary> Updates the Polygon. </summary>
		/// <remarks> This is normally not called by the "user". It is called automatically by PhySketch. </remarks>
		virtual void update() {}

		const VertexVariance& GetVertexVariance() const;

		/// <summary> Creates a unity sized square, in scene coordinate space. </summary>
		/// <returns> The square. </returns>
		static Polygon* CreateSquare(CoordinateSystem cs);

		/// <summary> Creates a circle polygon. </summary>
		/// <param name="cs"> The coordinate system </param>
		/// <param name="center"> The center. </param>
		/// <param name="radius"> The radius. </param>
		/// <param name="num_segments"> The number of segments that compose the circle.  </param>
		/// <returns> The circle polygon. </returns>
		static Polygon* CreateCircle( CoordinateSystem cs, Vector2 center, float radius, int num_segments);
		static std::vector<Vector2> GetCircleVertices(Vector2 center, float radius, int num_segments);

	protected:
		void computeTransformationMatrix();

	protected:
		std::vector<Vector2> _vertices;
		std::vector<uint> _vertexIndexes;
		//std::vector<std::pair<Material, int> > _materials; // TODO later :) the int thing is to define to wich vertexIndex the material applies
		bool _hasNewVertices;

		float _angle;
		Vector2 _position;
		Vector2 _scale;

		Matrix3 _transformMatrix;

		CoordinateSystem _coordSystem;	// the type of coordinates in wich vertices are defined
		DrawingMode _drawingMode;		
		VertexVariance _vertexVariance;
		AABB _aabb;

		UINT _vertexBuffer;	
		UINT _elementBuffer;	
		
	};
}
#endif // PhySketchPolygon_h__