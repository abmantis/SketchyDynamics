#ifndef PhySketchBodyPolygon_h__
#define PhySketchBodyPolygon_h__

#include "PhySketchPolygon.h"

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
		
	protected:		
		
	};
}
#endif // PhySketchBodyPolygon_h__