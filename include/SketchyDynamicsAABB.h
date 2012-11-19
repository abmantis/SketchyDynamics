#pragma once
#ifndef SketchyDynamicsAABB_h__
#define SketchyDynamicsAABB_h__

#include "SketchyDynamicsVector2.h"
#include "SketchyDynamicsMatrix3.h"

namespace SketchyDynamics
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

		/// <summary> Updates this AABB to accomodate the given AABB. </summary>
		/// <param name="aabb"> The AABB to accomodate in this AABB. </param>
		void update(AABB aabb)
		{
			if(!aabb._valid)
				return;

			update(aabb._min);
			update(aabb._max);
		}

		void transform(Vector2 position, Vector2 scale, float angle)
		{
			if(_valid)
			{
				Vector2 p1, p2, p3, p4;

				//Compute and rotate the four vertices of this box
				p1 = _min;
				p2 = Vector2(_max.x, _min.y);
				p3 = _max;
				p4 = Vector2(_min.x, _max.y);
				
				p1 = Vector2::Rotate(p1, angle);
				p2 = Vector2::Rotate(p2, angle);
				p3 = Vector2::Rotate(p3, angle);
				p4 = Vector2::Rotate(p4, angle);
				
				// invalidate this AABB and re-create it using the new rotated four vertices
				invalidate();
				update(p1);
				update(p2);
				update(p3);
				update(p4);

				// translate and scale the new AABB
 				_min = (_min + position) * scale;
 				_max = (_max + position) * scale;
			}
		}

		void transform(Matrix3 mat)
		{
			if(_valid)
			{
				Vector2 p1, p2, p3, p4;

				//Compute and transform the four vertices of this box
				p1 = _min;
				p2 = Vector2(_max.x, _min.y);
				p3 = _max;
				p4 = Vector2(_min.x, _max.y);

				p1 = mat * p1;
				p2 = mat * p2;
				p3 = mat * p3;
				p4 = mat * p4;

				// invalidate this AABB and re-create it using the new transformed vertices
				invalidate();
				update(p1);
				update(p2);
				update(p3);
				update(p4);
			}
		}

		bool isPointInside(const Vector2& pt)
		{
			if(pt > _min && pt < _max)
			{
				return true;
			}
			return false;
		}

	protected:
		bool _valid;
		Vector2 _min;
		Vector2 _max;
	};
}
#endif // SketchyDynamicsAABB_h__
