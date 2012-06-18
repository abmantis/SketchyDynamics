#pragma once
#ifndef Vector2_h__
#define Vector2_h__

#include "PhySketchDefinitions.h"
#include "PhySketchUtils.h"
#include "Box2D/Common/b2Math.h"

namespace PhySketch
{
	class Vector2
	{
	

	public:
		Vector2(void) : x(0), y(0)
		{
		}

		Vector2(float x, float y) : x(x), y(y)
		{
		}

		Vector2(const Vector2 &vector) : x(vector.x), y(vector.y)
		{
		}
		
		Vector2(const b2Vec2 &b2vector) : x(b2vector.x), y(b2vector.y)
		{
		}

		virtual ~Vector2(void)
		{
		}


		virtual bool operator==(const Vector2& right) const
		{
			return (x == right.x) && (y == right.y);
		}
		virtual bool operator!=(const Vector2& right) const
		{
			return !((*this) == right);
		}
		virtual bool operator<(const Vector2& right) const
		{
			return (x < right.x) && (y < right.y);
		}
		virtual bool operator>(const Vector2& right) const
		{
			return (x > right.x) && (y > right.y);
		}
		virtual bool operator<=(const Vector2& right) const
		{
			return (x <= right.x) && (y <= right.y);
		}
		virtual bool operator>=(const Vector2& right) const
		{
			return (x >= right.x) && (y >= right.y);
		}

		virtual Vector2 operator*(const Vector2& right) const
		{
			return Vector2(x * right.x, y * right.y);
		}	
		virtual Vector2 operator/(const Vector2& right) const
		{
			return Vector2(x / right.x, y / right.y);
		}
		virtual Vector2 operator+(const Vector2& right) const
		{
			return Vector2(x + right.x, y + right.y);
		}	
		virtual Vector2 operator-(const Vector2& right) const
		{
			return Vector2(x - right.x, y - right.y);
		}

		virtual Vector2& operator*=(const Vector2& right) 
		{
			this->x *= right.x;
			this->y *= right.y;
			return *this;
		}	
		virtual Vector2& operator/=(const Vector2& right)
		{
			this->x /= right.x;
			this->y /= right.y;
			return *this;
		}
		virtual Vector2& operator+=(const Vector2& right)
		{
			this->x += right.x;
			this->y += right.y;
			return *this;
		}	
		virtual Vector2& operator-=(const Vector2& right)
		{
			this->x -= right.x;
			this->y -= right.y;
			return *this;
		}

		virtual Vector2& operator=(const b2Vec2& right)
		{
			this->x = right.x;
			this->y = right.y;
			return *this;
		}

		virtual Vector2 operator*(const float& scalar) const
		{
			return Vector2(x * scalar, y * scalar);
		}	
		virtual Vector2 operator/(const float& scalar) const
		{
			return Vector2(x / scalar, y / scalar);
		}
		virtual Vector2 operator+(const float& scalar) const
		{
			return Vector2(x + scalar, y + scalar);
		}	
		virtual Vector2 operator-(const float& scalar) const
		{
			return Vector2(x - scalar, y - scalar);
		}

		virtual float dotProduct(Vector2 vec) const
		{
			return x * vec.x + y * vec.y;
		}

		virtual Vector2 normalised() const
		{
			return (*this) / length();
		}

		// Squared length of this vector
		virtual float sqrdLength() const
		{
			return x*x + y*y;
		}

		// Length of this vector
		virtual float length() const
		{
			return std::sqrt( sqrdLength() );
		}

		// Squared distance between this vector and vector p
		virtual float sqrdDistanceTo(const Vector2& p) const
		{
			return sqrdDistance(*this, p);
		}

		// Distance between this vector and vector p
		virtual float distanceTo(const Vector2& p) const
		{
			return distance(*this, p);
		}

		// Squared distance between two vectors
		static float sqrdDistance(const Vector2& p1, const Vector2& p2)
		{
			return (p2 - p1).sqrdLength();
		}

		// Distance between two vectors
		static float distance(const Vector2& p1, const Vector2& p2)
		{
			return (p2 - p1).length();
		}

		/// <summary> Compute the angle between two vectors. </summary>
		/// <param name="v1"> The first Vector2; </param>
		/// <param name="v2"> The second  Vector2 </param>
		/// <returns> The angle </returns>
		static float angleBetween(const Vector2& v1, const Vector2& v2)
		{
			float len = v1.length() * v2.length();
			
			// prevent divide by zero 
			if(len < 1e-7f)
			{
				len = 1e-7f;
			}

			float val = v1.dotProduct(v2) / len;

			return radiansToDegrees(acos(val));
		}

		friend std::ostream& operator<<(std::ostream& out, const Vector2& vector)
		{
			out << "(" << vector.x << ", " << vector.y << ")";
			return out;
		}

	public:
		float x;
		float y;

	public:
		static const Vector2 ZERO;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
		static const Vector2 NEGATIVE_UNIT_X;
		static const Vector2 NEGATIVE_UNIT_Y;
		static const Vector2 UNIT_SCALE;
	};
} // namespace PhySketch

#endif // Vector2_h__

