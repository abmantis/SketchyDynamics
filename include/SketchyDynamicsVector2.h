#pragma once
#ifndef Vector2_h__
#define Vector2_h__

#include "SketchyDynamicsDefinitions.h"
#include "Box2D/Common/b2Math.h"

namespace SketchyDynamics
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

		friend Vector2 operator/(const float& scalar, const Vector2& vector) 
		{
			return Vector2(scalar / vector.x, scalar / vector.y);
		}
		friend Vector2 operator-(const float& scalar, const Vector2& vector) 
		{
			return Vector2(scalar - vector.x, scalar - vector.y);
		}

		virtual b2Vec2 tob2Vec2() const
		{
			return b2Vec2(x, y);
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

		/// <summary> Compute the angle from this vector to another. </summary>
		/// <remarks> The angle can be positive or negative, depending on weather
		/// 	this vector is ahead of the other or not. </remarks>
		/// <param name="v"> The second  Vector2. </param>
		/// <returns> The angle. </returns>
		virtual float angleTo(const Vector2& v) const
		{
			float angle = atan2(v.y, v.x) - atan2(y, x);
			if(angle < -M_PI)
			{
				angle += 2.0f * M_PI;
			}
			else if (angle > M_PI)
			{
				angle -= 2.0f * M_PI;
			}
			return angle;
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
		/// <remarks> The angle is allways positive </remarks>
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

			return acos(val);
		}

		/// <summary> Compute the angle between a line and the horizontal axis. </summary>
		/// <param name="p1"> The first point that defines the line; </param>
		/// <param name="p2"> The seccond point that defines the line; </param>
		/// <returns> The angle </returns>
		static float lineAngle(const Vector2& p1, const Vector2& p2)
		{
			return atan2(p2.y - p1.y, p2.x - p1.x);
		}

		static Vector2 Vector2::Rotate(Vector2 p, float angle)
		{ 
			float xold,yold;
			Vector2 retP;
			xold=p.x;
			yold=p.y;
			retP.x= xold*cos(angle)-yold*sin(angle);
			retP.y= xold*sin(angle)+yold*cos(angle);
			return retP;
		}

		static Vector2 lineNormal(const Vector2& v1, const Vector2& v2)
		{
			 return Vector2(-(v2.y - v1.y), v2.x - v1.x);
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
		static const Vector2 ZERO_XY;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
		static const Vector2 UNIT_XY;
		static const Vector2 NEGATIVE_UNIT_X;
		static const Vector2 NEGATIVE_UNIT_Y;
		static const Vector2 NEGATIVE_UNIT_XY;		
	};
} // namespace SketchyDynamics

#endif // Vector2_h__

