#ifndef PhySketchPhysicsBody_h__
#define PhySketchPhysicsBody_h__

#include "PhySketchDefinitions.h"
#include "Box2D/Dynamics/b2Body.h"

namespace PhySketch
{
	class PhysicsBody 
	{	
		friend class PhysicsManager;
	public:
		PhysicsBody();
		PhysicsBody(b2Body *body);
		virtual ~PhysicsBody();
		

		/// <summary> Updates . </summary>
		/// <remarks> This is normally not called by the "user". It is called automatically by PhySketch. </remarks>
		virtual void update();
		
		/// <summary> Gets the box2D body. </summary>
		/// <returns> The box2D body. </returns>
		b2Body* getBox2DBody();

		/// <summary> Sets the box2D body. </summary>
		/// <param name="body"> The box2D body. </param>
		void setBox2DBody(b2Body* body);

		void reconstructPolygons();

	protected:
		bool _needsPolygonUpdate;
		b2Body* _body;
		std::vector<Polygon*> _polygons;
		std::vector<Polygon*> _oldPolygons;
	};
}
#endif // PhySketchPhysicsBody_h__