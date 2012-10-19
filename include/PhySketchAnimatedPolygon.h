#ifndef PhySketchAnimatedPolygon_h__
#define PhySketchAnimatedPolygon_h__

#include "PhySketchDefinitions.h"
#include "PhySketchPolygon.h"

namespace PhySketch
{
	//////////////////////////////////////////////////////////////////////////
	/// <summary> Animated polygon class. </summary>
	class AnimatedPolygon : public Polygon
	{
	public:
		AnimatedPolygon(VertexVariance vv = VV_Static, std::string name = "");
		virtual ~AnimatedPolygon();

		virtual void translate( const Vector2& amount, float unitsPerSec );
		virtual void setPosition( const Vector2& position, float unitsPerSec );

		virtual void update( ulong timeSinceLastFrame );		

	protected:
		Vector2 _desiredPosition;
		Vector2 _translationDirection;
		float _translationUnitsPerMs;
		ulong _translationMsToComplete;	
		
	};

	


}
#endif // PhySketchAnimatedPolygon_h__