#ifndef PhySketchRenderer_h__
#define PhySketchRenderer_h__

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"
#include "PhySketchVector2.h"

namespace PhySketch
{
	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer();
		virtual ~Renderer();

		/// <summary> Adds a polygon to the rendering list. </summary>
		/// <param name="polygon"> The new polygon. </param>
		virtual void addPolygon(Polygon *polygon);

		/// <summary> Removes the polygon from the rendering list. </summary>
		/// <param name="polygon"> The polygon to remove. </param>
		/// <remarks> This function will not clear any of the polygone's resources </remakrs>
		virtual void removePolygon(Polygon *polygon);
	
		/// <summary> Renders the polygons in the polygon list to screen. </summary>		
		virtual void render() const;

		virtual Vector2 getViewAxisLimits() const;

		static Renderer* getSingletonPtr(void);
		static Renderer& getSingleton(void);

	protected:
		std::set<Polygon*> _polygons;
		Vector2 _viewAxisLimits;		// The limits of the orthographic viewing region

		typedef std::set<Polygon*>::iterator polygon_set_iterator;
		
	};
}
#endif // PhySketchRenderer_h__