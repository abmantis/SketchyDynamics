#include "PhySketchAnimatedPolygon.h"

namespace PhySketch
{
AnimatedPolygon::AnimatedPolygon( VertexVariance vv /*= VV_Static*/, std::string name /*= ""*/ ) :
	Polygon(vv, name),
	_translationMsToComplete(0),
	_blinkCurrStateMsToComplete(0)
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
	// translate
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

	// blink
	if(_blinkCurrStateMsToComplete > 0)
	{		
		if(_blinkCurrStateMsToComplete > timeSinceLastFrame)
		{
			_blinkCurrStateMsToComplete -= timeSinceLastFrame;
		}
		else
		{
			// toggle visibility
			if(_alpha == 0)
			{
				_alpha = 1.0f;
				_blinkCurrStateMsToComplete = _blinkMsOn;
			}
			else
			{
				_alpha = 0.0f;
				_blinkCurrStateMsToComplete = _blinkMsOff;
			}
		}

	}
}

void AnimatedPolygon::blink( ulong timeOn, ulong timeOff )
{
	_blinkMsOn = timeOn;
	_blinkMsOff = timeOff;

	_blinkOriginalAlpha = _alpha;

	// toggle visibility
	if(_alpha == 0)
	{
		_alpha = 1.0f;
		_blinkCurrStateMsToComplete = _blinkMsOn;
	}
	else
	{
		_alpha = 0.0f;
		_blinkCurrStateMsToComplete = _blinkMsOff;
	}
}

void AnimatedPolygon::stopBlink( bool restoreAlpha )
{
	_blinkCurrStateMsToComplete = 0;
	_alpha = _blinkOriginalAlpha;
}




}

