#ifndef PhySketchBodyPolygon_h__
#define PhySketchBodyPolygon_h__

#include "PhySketchPolygon.h"
#include "Box2D/Dynamics/b2Body.h"

namespace PhySketch
{
	class BodyPolygon : public Polygon
	{	
	public:
		BodyPolygon(DrawingMode dm = DM_LINES);
		virtual ~BodyPolygon();
		

		/// <summary> Updates the Polygon. </summary>
		/// <remarks> This is normally not called by the "user". It is called automatically by PhySketch. </remarks>
		virtual void update();
		
		/// <summary> Gets the physics body. </summary>
		/// <returns> The physics body. </returns>
		const b2Body* getPhysicsBody(void) const;

		/// <summary> Sets the physics body. </summary>
		/// <param name="body"> The body. </param>
		void setPhysicsBody(b2Body* body);


	protected:
		b2Body* _physicsBody;
	};
}
#endif // PhySketchBodyPolygon_h__