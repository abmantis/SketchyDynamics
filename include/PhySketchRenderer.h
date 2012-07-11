#ifndef PhySketchRenderer_h__
#define PhySketchRenderer_h__

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"
#include "PhySketchVector2.h"
#include "PhySketchPolygon.h"

namespace PhySketch
{
	/// <summary> Scene query callback abstract class. </summary>
	class SceneQueryCallback
	{
	public:

		/// <summary> Reports an intersected polygon. </summary>
		/// <param name="p"> The intersected Polygon . </param>
		/// <returns> true to continue querying other for more Polygons. </returns>
		virtual bool reportPolygon(Polygon *p) = 0;
	};

	/// <summary> Scene query callback implementation that stores the first
	/// 	intersected object. </summary>
	class FirstObjectSceneQueryCallback : public SceneQueryCallback
	{
	public:
		FirstObjectSceneQueryCallback(Vector2 pt) : 
		  _point(pt),
			  _firstPolygon(nullptr)
		  {
		  }

		  virtual bool reportPolygon(Polygon *p) 
		  {
			  if(p->isPointInside(_point))
			  {
				  _firstPolygon = p;
				  return false;
			  }

			  return true;
		  }

		  Vector2 _point;
		  Polygon* _firstPolygon;

	};

	/// <summary> Scene query callback implementation that stores an array of
	/// 	intersected objects up to a established number of objects. The
	/// 	objects are storend front-to-back (closest first). </summary>
	class SimpleSceneQueryCallback : public SceneQueryCallback
	{
	public:
		SimpleSceneQueryCallback(Vector2 pt, uint maxPolyCount) : 
		  _point(pt),
			  _maxPolyCount(maxPolyCount),
			  _intersectedPolyCount(0)
		  {
		  }

		  virtual bool reportPolygon(Polygon *p) 
		  {
			  if(p->isPointInside(_point))
			  {
				  _intersectedPolygons.push_back(p);
				  ++_intersectedPolyCount;
			  }

			  if(_intersectedPolyCount >= _maxPolyCount)
			  {
				  return false;
			  }

			  return true;
		  }

		  uint _maxPolyCount;
		  uint _intersectedPolyCount;
		  Vector2 _point;
		  std::vector<Polygon*> _intersectedPolygons;

	};

	enum RenderQueueType
	{
		RQT_Background,
		RQT_Scene,
		RQT_UI
	};

	class Renderer : public Singleton<Renderer>
	{
		friend class ApplicationWindow_WGL;
		
		struct RenderQueueParams
		{
			RenderQueueParams(Polygon *p, ulong depth) : polygon(p), depth(depth) {}

			Polygon *polygon;
			ulong depth;
		};
		typedef std::list<RenderQueueParams> RenderQueue;
		typedef std::unordered_map<std::string, Polygon*> NamedPolygonMap;

	public:
		Renderer();
		virtual ~Renderer();

		/// <summary> Adds a polygon to the rendering list. </summary>
		/// <param name="polygon"> The new polygon. </param>
		/// <param name="depth"> The depth/position of the polygon in the queue. </param>
		/// <param name="rq"> Which render queue the polygon should be added to. </param>
		virtual void addPolygon(Polygon *polygon, ulong depth, RenderQueueType rq = RQT_Scene);
	
		/// <summary> Adds a polygon to the rendering list. </summary>
		/// <param name="polygon"> The new polygon. </param>
		/// <param name="rq"> Which render queue the polygon should be added to. </param>
		virtual void addPolygon(Polygon *polygon, RenderQueueType rq = RQT_Scene);

		/// <summary> Removes the polygon from the rendering list. </summary>
		/// <param name="polygon"> The polygon to remove. </param>
		/// <param name="rq"> Which render queue the polygon should be added to. </param>
		virtual void removePolygon(Polygon *polygon, RenderQueueType rq = RQT_Scene);

		virtual Polygon* getPolygonByName(std::string name);

		/// <summary> Find polygons whose AABBs are intersected by a specific
		/// 	point. </summary>
		/// <remarks> This only checks for AABB intersection and not pixel
		/// 	intersection. The polygons are reported by the order they appear
		/// 	on screen, i.e. the first reported polygon appears above all
		/// 	others. </remarks>
		/// <param name="pt"> The intersecting point. </param>
		/// <param name="callback"> The callback to report intersected polygons. </param>
		virtual void queryScene(const Vector2& pt, SceneQueryCallback *callback );
	
		/// <summary> Renders the polygons in the polygon list to screen. </summary>		
		virtual void render();

		/// <summary> Gets the minimum limit for the current view of the scene. </summary>
		/// <returns> The scene view axis minimum limit. </returns>
		virtual Vector2 getSceneViewAxisMin() const;

		/// <summary> Gets the maximum limit for the current view of the scene. </summary>
		/// <returns> The scene view axis maximum limit. </returns>
		virtual Vector2 getSceneViewAxisMax() const;

		/// <summary> Converts a position in screen coordinates to scene coordinates. </summary>
		/// <param name="vec"> The vector in screen coordinates. </param>
		/// <returns> The vector in scene coordinates.  </returns>
		virtual Vector2 windowToScene(const Vector2 &vec);

		static Renderer* getSingletonPtr(void);
		static Renderer& getSingleton(void);

	protected:
		virtual bool initGL();

		virtual void renderPolygons();
		virtual void renderPolygon(Polygon *poly) const;

		virtual void updateOpenGLBuffers(SubPolygon *subpolygon, VertexVariance variance) const;

		virtual RenderQueue* getRenderQueuePtr(RenderQueueType rqp);
		

	protected:
		RenderQueue _backgroundRenderQueue;
		RenderQueue _sceneRenderQueue;
		RenderQueue _uiRenderQueue;
		
		NamedPolygonMap _namedPolygonMap;

		Vector2 _sceneViewMin;		// The minimum limit of the current scene viewing region (only for CT_Scene polygons)
		Vector2 _sceneViewMax;		// The minimum limit of the current scene viewing region (only for CT_Scene polygons)
		Vector2 _viewportSize;
		Vector2 _windowSize;

		Shader_GL* _mainVertexShader;
		Shader_GL* _mainFragmentShader;
		ShaderProgram_GL* _mainShaderProgram;

		struct
		{
			struct 
			{
				GLint position;
				GLint textCoord;
			} attributes;
			
			struct
			{
				GLint transformation;
				GLint color;
				GLint textureFlag;
				GLint texture;
			} uniforms;

		} _shaderVars;		
	};
}
#endif // PhySketchRenderer_h__