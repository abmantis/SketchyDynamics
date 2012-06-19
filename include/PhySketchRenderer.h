#ifndef PhySketchRenderer_h__
#define PhySketchRenderer_h__

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"
#include "PhySketchVector2.h"

namespace PhySketch
{
	class Renderer : public Singleton<Renderer>
	{
		friend class ApplicationWindow_WGL;
	public:
		Renderer();
		virtual ~Renderer();

		/// <summary> Adds a polygon to the rendering list. </summary>
		/// <param name="polygon"> The new polygon. </param>
		virtual void addPolygon(Polygon *polygon);

		/// <summary> Removes the polygon from the rendering list. </summary>
		/// <param name="polygon"> The polygon to remove. </param>
		/// <remarks> This function will not clear any of the polygon's resources </remakrs>
		virtual void removePolygon(Polygon *polygon);
	
		/// <summary> Renders the polygons in the polygon list to screen. </summary>		
		virtual void render() const;

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

		virtual void renderScenePolygons() const;
		virtual void renderPolygon(Polygon *poly) const;

		virtual void updateOpenGLBuffers(Polygon *polygon) const;
		

	protected:
		std::set<Polygon*> _scenePolygons;

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
			} attributes;
			
			struct
			{
				GLint transformation;
				GLint color;
			} uniforms;

		} _shaderVars;

		typedef std::set<Polygon*>::iterator polygon_set_iterator;
		
	};
}
#endif // PhySketchRenderer_h__