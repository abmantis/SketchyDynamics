#include "PhySketchAnimatedPolygon.h"

namespace PhySketch
{
AnimatedPolygon::AnimatedPolygon( VertexVariance vv /*= VV_Static*/, std::string name /*= ""*/ ) :
	Polygon(vv, name),
	_translationMsToComplete(0)
{
}

AnimatedPolygon::~AnimatedPolygon()
{
}

void AnimatedPolygon::translate( const Vector2& amount, float unitsPerSec )
{	
	float length				= amount.length();
	_translationDirection		= amount / length;	// normalise
	_translationUnitsPerMs		= unitsPerSec / 1000.0f;
	_translationMsToComplete	= static_cast<ulong>((length / unitsPerSec) * 1000.0f) ;
	_desiredPosition			= _position + amount;
	
}

void AnimatedPolygon::setPosition( const Vector2& position, float unitsPerSec  )
{
	translate(position - _position, unitsPerSec);
}

void AnimatedPolygon::update( ulong timeSinceLastFrame )
{
	if(_translationMsToComplete > 0)
	{		
		if(_translationMsToComplete > timeSinceLastFrame)
		{
			_translationMsToComplete -= timeSinceLastFrame;
			Polygon::translate(_translationDirection * _translationUnitsPerMs * (float)timeSinceLastFrame);
		}
		else
		{
			_translationMsToComplete = 0;
			Polygon::setPosition(_desiredPosition);
		}		
	}
}



}

