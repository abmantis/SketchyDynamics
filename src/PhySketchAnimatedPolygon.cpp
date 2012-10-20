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
		if(_blinkFading)
		{
			_alpha += _blinkCurrIncrePerMs;
			if(_alpha < 0.0f)
			{
				_alpha = 0.0f;
			}
			else if(_alpha > _blinkOriginalAlpha)
			{
				_alpha = _blinkOriginalAlpha;
			}
		}

		if(_blinkCurrStateMsToComplete > timeSinceLastFrame)
		{
			_blinkCurrStateMsToComplete -= timeSinceLastFrame;
		}
		else
		{
			// toggle visibility
			if(_blinkFading)
			{				
				if(_blinkFadingIn)
				{
					_blinkCurrStateMsToComplete = _blinkMsOn;
					_alpha = _blinkOriginalAlpha;
				}
				else
				{
					_blinkCurrStateMsToComplete = _blinkMsOff;
					_alpha = 0.0f;
				}
			}
			else
			{
				if(_blinkFadingIn)
				{
					_blinkCurrStateMsToComplete = _blinkMsFadeOut;
					_blinkCurrIncrePerMs		= _blinkFadeOutIncrePerMs;
				}
				else
				{
					_blinkCurrStateMsToComplete = _blinkMsFadeIn;
					_blinkCurrIncrePerMs		= _blinkFadeInIncrePerMs;
				}
				_blinkFadingIn = !_blinkFadingIn;
			}
			_blinkFading = !_blinkFading;
		}

	}
}

void AnimatedPolygon::blink( ulong timeOn, ulong timeOff, ulong fadeInTime, ulong fadeOutTime )
{
	_blinkOriginalAlpha			= _alpha;
	_blinkMsOn					= timeOn;
	_blinkMsOff					= timeOff;
	_blinkMsFadeIn				= fadeInTime;
	_blinkMsFadeOut				= fadeOutTime;
	_blinkFadeInIncrePerMs		= _blinkOriginalAlpha / fadeInTime;
	_blinkFadeOutIncrePerMs		= (_blinkOriginalAlpha / fadeOutTime) * -1.0f;

	_blinkFading = true;

	// toggle visibility
	if(_alpha == 0)
	{
		_blinkCurrStateMsToComplete	= _blinkMsFadeIn;
		_blinkCurrIncrePerMs		= _blinkFadeInIncrePerMs;
		_blinkFadingIn				= true;
	}
	else
	{
		_blinkCurrStateMsToComplete	= _blinkMsFadeOut;
		_blinkCurrIncrePerMs		= _blinkFadeOutIncrePerMs;
		_blinkFadingIn				= false;
	}
}

void AnimatedPolygon::stopBlink( bool restoreAlpha )
{
	_blinkCurrStateMsToComplete = 0;
	_alpha = _blinkOriginalAlpha;
}




}

