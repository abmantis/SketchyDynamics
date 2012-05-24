#pragma once
#ifndef Vector2_h__
#define Vector2_h__

#include "PhySketchDefinitions.h"

namespace PhySketch
{
	class Vector2
	{
	public:
		Vector2(void) : x(0), y(0)
		{
		}

		Vector2(double x, double y) : x(x), y(y)
		{
		}

		Vector2(const Vector2 &point) : x(point.x), y(point.y)
		{
		}

		virtual ~Vector2(void)
		{
		}


		virtual bool operator==(const Vector2& point) const
		{
			return (x == point.x) && (y == point.y);
		}
		virtual bool operator<(const Vector2& point) const
		{
			return (x < point.x) && (y < point.y);
		}
		virtual bool operator>(const Vector2& point) const
		{
			return (x > point.x) && (y > point.y);
		}
		virtual bool operator<=(const Vector2& point) const
		{
			return (x <= point.x) && (y <= point.y);
		}
		virtual bool operator>=(const Vector2& point) const
		{
			return (x >= point.x) && (y >= point.y);
		}

		virtual Vector2 operator*(const Vector2& point) const
		{
			return Vector2(x * point.x, y * point.y);
		}	
		virtual Vector2 operator/(const Vector2& point) const
		{
			return Vector2(x / point.x, y / point.y);
		}
		virtual Vector2 operator+(const Vector2& point) const
		{
			return Vector2(x + point.x, y + point.y);
		}	
		virtual Vector2 operator-(const Vector2& point) const
		{
			return Vector2(x - point.x, y - point.y);
		}

		virtual Vector2 operator*(const double& scalar) const
		{
			return Vector2(x * scalar, y * scalar);
		}	
		virtual Vector2 operator/(const double& scalar) const
		{
			return Vector2(x / scalar, y / scalar);
		}
		virtual Vector2 operator+(const double& scalar) const
		{
			return Vector2(x + scalar, y + scalar);
		}	
		virtual Vector2 operator-(const double& scalar) const
		{
			return Vector2(x - scalar, y - scalar);
		}


		// Squared length of this vector
		virtual double sqrdLength() const
		{
			return x*x + y*y;
		}

		// Length of this vector
		virtual double length() const
		{
			return std::sqrt( sqrdLength() );
		}

		// Squared distance between this point and point p
		virtual double sqrdDistanceTo(const Vector2& p) const
		{
			return sqrdDistance(*this, p);
		}

		// Distance between this point and point p
		virtual double distanceTo(const Vector2& p) const
		{
			return distance(*this, p);
		}

		// Squared distance between two points
		static double sqrdDistance(const Vector2& p1, const Vector2& p2)
		{
			return (p2 - p1).sqrdLength();
		}

		// Distance between two points
		static double distance(const Vector2& p1, const Vector2& p2)
		{
			return (p2 - p1).length();
		}

		friend std::ostream& operator<<(std::ostream& out, const Vector2& point)
		{
			out << "(" << point.x << ", " << point.y << ")";
			return out;
		}

	public:
		double x;
		double y;
	};
} // namespace PhySketch

#endif // Vector2_h__

