#pragma once
#ifndef PhySketchAABB_h__
#define PhySketchAABB_h__

#include "PhySketchVector2.h"

namespace PhySketch
{
	class AABB
	{
	public:
		AABB()
		{
			_valid = false;
		}

		AABB(Vector2 min, Vector2 max) : _min(min), _max(max)
		{
			if(_min <= _max)
			{
				_valid = true;
			}
			else
			{
				_valid = false;
			}
		}

		bool isValid()
		{
			return _valid;
		}

		/// <summary> Invalidates the AABB so that it is considered as new in the next update. </summary>
		void invalidate()
		{
			_valid = false;
		}

		Vector2 getMin() const 
		{ 
			return _min; 
		}
		Vector2 getMax() const
		{ 
			return _max;
		}

		// The height of the bounding box (along the vertical YY axis)
		float getHeight() const
		{
			return _max.y - _min.y; 
		}
		// The width of the bounding box (along the horizontal XX axis)
		float getWidth() const
		{
			return _max.x - _min.x; 
		}
		Vector2 getSize() const
		{
			return _max - _min;
		}

		// The center of the bounding box
		Vector2 getCenter() const
		{
			return (_min + _max) * 0.5f;
		}

		// Update the AABB (if needed) to accommodate Vector2 p
		void update(Vector2 p)
		{
			if(_valid)
			{
				if(p.x < _min.x)
				{
					_min.x = p.x;
				}
				else if(p.x > _max.x)
				{
					_max.x = p.x;
				}

				if(p.y < _min.y)
				{
					_min.y = p.y;
				}
				else if(p.y > _max.y)
				{
					_max.y = p.y;
				}			
			}
			else
			{
				_min = p;
				_max = p;
				_valid = true;
			}
		}

	protected:
		bool _valid;
		Vector2 _min;
		Vector2 _max;
	};
}
#endif // PhySketchAABB_h__
