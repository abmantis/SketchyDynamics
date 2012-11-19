#ifndef SketchyDynamicsPolygon_h__
#define SketchyDynamicsPolygon_h__

#include "SketchyDynamicsDefinitions.h"
#include "SketchyDynamicsVector2.h"
#include "SketchyDynamicsAABB.h"
#include "SketchyDynamicsMatrix3.h"
#include "SketchyDynamicsMaterial.h"

#define SKETCHYDYNAMICS_POLYGON_UTYPE_NONE		0
#define SKETCHYDYNAMICS_POLYGON_UTYPE_PHYBODY		1
#define SKETCHYDYNAMICS_POLYGON_UTYPE_PHYJOINT	2
#define SKETCHYDYNAMICS_POLYGON_UTYPE_GUESSESLIST	3
#define SKETCHYDYNAMICS_POLYGON_UTYPE_USER		4


namespace SketchyDynamics
{
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

	//////////////////////////////////////////////////////////////////////////
	/// <summary> SubPolygon class. Represents a (sub)polygon that is part of
	/// 	a main polygon. </summary>
	class SubPolygon
	{
		friend class Polygon;
		friend class Renderer;

	protected:	
		SubPolygon(DrawingMode dm);
		virtual ~SubPolygon() {};

	public:
		/// <summary> Adds a new vertex . </summary>
		/// <param name="vertex"> The vertex. </param>
		virtual void addVertex(const Vector2& vertex);

		virtual void addVertex(const Vector2& vertex, const Vector2& textureCoord);

		/// <summary> Gets an vector with the ordered vertices for this
		/// 	SubPolygon. </summary>
		/// <remarks> This functions gives all the vertices (even repeated ones)
		/// 	without the need for an element (vertex indexes) array. </remarks>
		/// <param name="vertices"> The vector to old the vertices. </param>
		virtual void getOrderedVertices(std::vector<Vector2> &vertices);

		/// <summary> Gets this Polygon's material. </summary>
		/// <returns> The material. </returns>
		virtual Material* getMaterial(void) const;

		/// <summary> Sets this Polygon's material. </summary>
		/// <param name="material"> The material. </param>
		virtual void setMaterial(Material* material);

		/// <summary> Gets the drawing mode. </summary>
		/// <returns> The drawing mode. </returns>
		virtual const DrawingMode& getDrawingMode() const;

		/// <summary> Query if a point is inside this polygon. </summary>
		/// <param name="pt"> The point. </param>
		/// <returns> true if point is inside, false if not. </returns>
		virtual bool isPointInside(const Vector2& pt) const;

		/// <summary> Optimize vertex list, by removing duplicated and unreferenced vertices. </summary>
		//virtual void optimizeVertexList(); // TODO

		/// <summary> Updates the axis aligned bounding box. </summary>
		virtual void updateAABB();

		/// <summary> Gets the axis aligned bounding box in local space. </summary>
		/// <returns> The axis aligned bounding box. </returns>
		virtual const AABB& getAABB() const;

		/// <summary> Gets the axis aligned bounding box in world space. </summary>
		/// <returns> The axis aligned bounding box. </returns>
		virtual AABB getWorldAABB(bool bestFit, Matrix3 transformMatrix) const;		

		/// <summary> Sets the visibility flag of this SubPolygon. </summary>
		/// <param name="visible"> true to show, false to hide. </param>
		virtual void setVisible(bool visible);

		/// <summary> Query if this SubPolygon is visible. </summary>
		/// <returns> true if visible, false if not. </returns>
		virtual bool isVisible() const;

		virtual void recomputeTexCoordsToFit();
		virtual void recomputeTexCoordsAsVertices();
	protected:
		std::vector<Vector2> _vertices;
		std::vector<Vector2> _textureCoords;
		std::vector<uint> _vertexIndexes;
		Material* _material;
		DrawingMode _drawingMode;
		bool _hasNewVertices;
		AABB _aabb;
		bool _visible;

		UINT _vertexBuffer;	
		UINT _elementBuffer;
		UINT _texCoordBuffer;
	};

	//////////////////////////////////////////////////////////////////////////
	/// <summary> Polygon class. </summary>
	class Polygon 
	{
		friend class Renderer;

	public:
		Polygon(VertexVariance vv = VV_Static, std::string name = "", CoordinateSystem cs = CS_Scene);
		virtual ~Polygon();

		virtual std::string getName() const;

		/// <summary> Creates a new SubPolygon in this Polygon. </summary>
		/// <param name="dm"> The DrawingMode for this SubPolygon. </param>
		/// <returns> The new SubPolygon. </returns>
		virtual SubPolygon* createSubPolygon(DrawingMode dm);

		/// <summary> Creates a unity sized square, in scene coordinate space. </summary>
		/// <param name="dm"> The drawing mode. </param>
		/// <returns> The new square subpolygon. </returns>
		SubPolygon* CreateSquareSubPolygon(DrawingMode dm, Vector2 halfSize = Vector2(0.5f, 0.5f), Vector2 center = Vector2(0.0f, 0.0f));

		/// <summary> Creates a circle subpolygon. </summary>
		/// <param name="dm"> The drawing mode. </param>
		/// <param name="center"> The circle's center. </param>
		/// <param name="radius"> The radius. </param>
		/// <param name="num_segments"> The number of segments that compose the circle. </param>
		/// <returns> The new circle subpolygon. </returns>
		SubPolygon* CreateCircleSubPolygon( DrawingMode dm, Vector2 center, float radius, int num_segments);

		/// <summary> Gets the number of SubPolygons in this Polygon. </summary>
		/// <returns> The number of SubPolygon. </returns>
		virtual uint getSubPolygonCount() const;

		/// <summary> Gets a SubPolygon based on its index. </summary>
		/// <param name="i"> Zero-based index of the SubPolygon. </param>
		/// <returns> The SubPolygon. </returns>
		virtual SubPolygon* getSubPolygon(uint i);

		/// <summary> Gets the rotation angle of the polygon in radians. </summary>
		/// 
		/// <returns> The angle in radians. </returns>
		virtual float getAngle(void) const;

		/// <summary> Sets the rotation angle of the polygon in radians. </summary>
		///
		/// <param name="angle"> The new angle in radians. </param>
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
		/// <param name="amount"> The amount to translate the polygon. </param>
		virtual void translate(const Vector2& amount);

		/// <summary> Rotates by the given angle; </summary>
		/// <param name="angle"> The amount of rotation to be applied, in radians. </param>
		virtual void rotate(const float& angle);

		virtual void rotateAroundPoint(float angle, const Vector2& rotationPoint);

		/// <summary> Scales the polygon </summary>
		/// <param name="factor"> The scale factor. </param>
		virtual void scale(const Vector2& factor);
		
		/// <summary> Gets the coordinate system used by vertices. </summary>
		/// <returns> The coordinate system. </returns>
		virtual const CoordinateSystem& getCoordinateSystem() const;

		/// <summary> Updates the axis aligned bounding box. </summary>
		virtual void updateAABB();

		/// <summary> Gets the axis aligned bounding box in local space. </summary>
		/// <returns> The axis aligned bounding box. </returns>
		virtual const AABB& getAABB() const;

		/// <summary> Gets the axis aligned bounding box in world space. </summary>
		/// <returns> The axis aligned bounding box. </returns>
		virtual AABB getTransformedAABB(bool bestFit) const;		

		/// <summary> Sets a custom type to identify this polygon. </summary>
		/// <remarks> User-defined types should be greater that
		/// 	SKETCHYDYNAMICS_POLYGON_UTYPE_USER. Values below this are used by
		/// 	SketchyDynamics. </remarks>
		/// <param name="type"> The custom type. </param>
		virtual void setUserType(ulong type);

		virtual ulong getUserType() const;

		/// <summary> Sets custom user data. </summary>
		/// <param name="data"> The data. </param>
		virtual void setUserData(void* data);

		virtual void* getUserData();

		/// <summary> Query if a point is inside this polygon. </summary>
		/// <param name="pt"> The point. </param>
		/// <returns> true if point is inside, false if not. </returns>
		virtual bool isPointInside(const Vector2& pt) const;

		/// <summary> Sets the material for all SubPolygon. </summary>
		/// <param name="material"> The material. </param>
		virtual void setMaterial(Material* material);

		/// <summary> Sets the visibility flag of this Polygon. </summary>
		/// <param name="visible"> true to show, false to hide. </param>
		virtual void setVisible(bool visible);

		/// <summary> Query if this object is visible. </summary>
		/// <returns> true if visible, false if not. </returns>
		virtual bool isVisible() const;

		/// <summary> Updates the Polygon. </summary>
		/// <remarks> This is normally not called by the "user". It is called automatically by SketchyDynamics. </remarks>
		virtual void update(ulong timeSinceLastFrame) {}

		const VertexVariance& GetVertexVariance() const;

		virtual void recomputeTexCoordsToFit();
		virtual void recomputeTexCoordsAsVertices();

		static std::vector<Vector2> GetCircleVertices(Vector2 center, float radius, int num_segments);

	protected:
		void computeTransformationMatrix();
		void clearSubPolygons();


	protected:
		std::vector<SubPolygon*> _subPolygons;

		std::string _name;		

		float _angle;
		Vector2 _position;
		Vector2 _scale;

		Matrix3 _transformMatrix;

		CoordinateSystem _coordSystem;	// the type of coordinates in wich vertices are defined
		VertexVariance _vertexVariance;
		AABB _aabb;

		bool _visible;
		float _alpha;

		void* _userData;
		ulong _userType;			

		bool _inRenderingQueue;
		
	};

}
#endif // SketchyDynamicsPolygon_h__