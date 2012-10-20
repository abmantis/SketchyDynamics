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

		/// <summary> Blinks. </summary>
		/// <param name="timeOn"> The time (in ms) to stay on. </param>
		/// <param name="timeOff"> The time (in ms) to stay off. </param>
		virtual void blink(ulong timeOn, ulong timeOff);

		/// <summary> Stops blinking. </summary>
		virtual void stopBlink(bool restoreAlpha);

		virtual void update( ulong timeSinceLastFrame );		

	protected:
		// translation
		Vector2 _desiredPosition;
		Vector2 _translationDirection;
		float _translationUnitsPerMs;
		ulong _translationMsToComplete;

		// blink
		ulong _blinkMsOn;
		ulong _blinkMsOff;
		ulong _blinkCurrStateMsToComplete;
		float _blinkOriginalAlpha;

		
	};

	


}
#endif // PhySketchAnimatedPolygon_h__