#pragma once
#ifndef PhySketchUtils_h__
#define PhySketchUtils_h__
#include "PhySketchVector2.h"


namespace PhySketch
{
	static std::string toString(float f)
	{
		std::ostringstream stream;
		stream << f;
		return stream.str();
	}

	static std::string toString(long l)
	{
		std::ostringstream stream;
		stream << l;
		return stream.str();
	}
	
	static std::string toString(unsigned long i)
	{
		std::ostringstream stream;
		stream << i;
		return stream.str();
	}

	static std::string toString(bool b)
	{
		if(b)
			return "1";
		return "0";
	}

	static float stringToFloat(std::string str)
	{
		std::istringstream stm;
		stm.str(str);
		float f;
		stm >> f;
		return f;
	}

	static long stringToLong(std::string str)
	{
		std::istringstream stm;
		stm.str(str);
		long l;
		stm >> l;
		return l;
	}

	static unsigned long stringToULong(std::string str)
	{
		std::istringstream stm;
		stm.str(str);
		unsigned long l;
		stm >> l;
		return l;
	}

	static float degreesToRadians (float degrees)
	{
		float radians = (M_PI / 180.0f) * degrees;
		return (radians);
	}

	static float radiansToDegrees (float radians)
	{
		float degrees = (180.0f / M_PI) * radians;
		return (degrees);
	}

	static long round(float f)
	{
		return (long)floor(f + 0.5);
	}

	/// <summary> Reads a file contents. </summary>
	/// <param name="filename"> Path to the file. </param>
	/// <param name="length"> [out] The length of the file. </param>
	/// <returns> null if it fails, else the file contents. </returns>
	static void* readFileContents(const char *filename, int &length)
	{
		FILE *f = fopen(filename, "r");
		void *buffer;

		if (!f) {
			fprintf(stderr, "Unable to open %s for reading\n", filename);
			return NULL;
		}

		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(length+1);
		length = fread(buffer, 1, length, f);
		fclose(f);
		((char*)buffer)[length] = '\0';

		return buffer;
	}

	static float sign(const Vector2& p1, const Vector2& p2, const Vector2& p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	static bool isPointInTri(const Vector2& pt, const Vector2& tri_v1, const Vector2& tri_v2, const Vector2& tri_v3)
	{
		bool b1, b2, b3;

		b1 = sign(pt, tri_v1, tri_v2) < 0.0f;
		b2 = sign(pt, tri_v2, tri_v3) < 0.0f;
		b3 = sign(pt, tri_v3, tri_v1) < 0.0f;

		return ((b1 == b2) && (b2 == b3));
	}

	/// <summary> Find the line-line intersection point. </summary>
	/// <param name="start1"> The line 1 start point. </param>
	/// <param name="end1"> The line 1 end point. </param>
	/// <param name="start2"> The line 2 start point. </param>
	/// <param name="end2"> The line 2 end point. </param>
	/// <param name="intersection"> [out] The intersection (if it exists). </param>
	/// <returns> True if an intersection point exists. </returns>
	static bool lineLineIntersection(const Vector2& start1, const Vector2& end1, const Vector2& start2, const Vector2& end2, Vector2 &intersection) 
	{
		// Store the values for fast access and easy
		// equations-to-code conversion
		float x1 = start1.x, x2 = end1.x, x3 = start2.x, x4 = end2.x;
		float y1 = start1.y, y2 = end1.y, y3 = start2.y, y4 = end2.y;

		float s1_x, s1_y, s2_x, s2_y;
		s1_x = x2 - x1;     s1_y = y2 - y1;
		s2_x = x4 - x3;     s2_y = y4 - y3;

		float s, t;
		s = (-s1_y * (x1 - x3) + s1_x * (y1 - y3)) / (-s2_x * s1_y + s1_x * s2_y);
		t = ( s2_x * (y1 - y3) - s2_y * (x1 - x3)) / (-s2_x * s1_y + s1_x * s2_y);

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
		{
			// intersection detected
			intersection.x = x1 + (t * s1_x);
			intersection.y = y1 + (t * s1_y);
			return true;
		}

		return false; // No intersection		
	}

	static bool checkSegmentSelfIntersection(const std::vector<Vector2> &segment)
	{
		Vector2 intersection;
		uint ptCnt = segment.size();
		uint lastVert = ptCnt - 1;
		uint i = 0;
		
		if(segment[0] == segment[lastVert])
		{
			// The first and last vertices are the same so we need to first
			// check the first line without the last, since they "intersect" 

			for (uint j = i+2; j < lastVert - 1 ; ++j)
			{
				if( lineLineIntersection(segment[i], segment[i+1], segment[j], segment[j+1], intersection) )
				{
					return true;
				}
			}

			i = 1;
		}

		for (; i < lastVert; ++i)
		{
			for (uint j = i+2; j < lastVert; ++j)
			{
				if( lineLineIntersection(segment[i], segment[i+1], segment[j], segment[j+1], intersection) )
				{
					return true;
				}
			}
		}
		return false;
	}
	
	static bool isCounterClockwise(const Vector2& p1, const Vector2& p2, const Vector2& p3)
	{
		return ((p2.y-p1.y)*(p3.x-p2.x)<(p3.y-p2.y)*(p2.x-p1.x));
	}

	static void rectangleArroundLine(const Vector2& line_p1, const Vector2& line_p2, 
		float tickness, Vector2& rect_p1, Vector2& rect_p2, Vector2& rect_p3, Vector2& rect_p4)
	{		
		Vector2 normal = Vector2::lineNormal(line_p1, line_p2).normalised() * tickness;
		rect_p1 = line_p1 + normal;
		rect_p2 = line_p1 - normal;
		rect_p3 = line_p2 + normal;
		rect_p4 = line_p2 - normal;
	}

	static float getScaleFactor(float currentScale, float desiredScale)
	{
		if (currentScale == 0.0f)
		{
			currentScale = FLT_MIN;
		}

		if(currentScale < desiredScale)
		{
			return 1.0f + (desiredScale - currentScale) * (1.0f / currentScale);
		}
		else
		{
			return 1.0f - (currentScale - desiredScale) * (1.0f / currentScale);
		}
	}

	static Vector2 getCentroid(const std::vector<Vector2> &points)
	{
		Vector2 p(0,0);

		size_t pointCount = points.size();
		for (size_t i = 0; i < pointCount; i++)
		{
			p = p + points[i];
		}

		p = p / (float) pointCount;

		return p;
	}

	static void translateCentroidTo(std::vector<Vector2> &pointsToTranslate, Vector2 newCentroidPos)
	{
		Vector2 c = getCentroid(pointsToTranslate);

		size_t pointCount = pointsToTranslate.size();
		for (size_t i = 0; i < pointCount; i++)
		{
			pointsToTranslate[i] = pointsToTranslate[i] + newCentroidPos - c;
// 			double qx = pointsToTranslate[i].x + newCentroidPos.x - c.x;
// 			double qy = pointsToTranslate[i].y + newCentroidPos.y - c.y;
// 
// 			pointsToTranslate[i].x = qx;
// 			pointsToTranslate[i].y = qy;
		}
	}
  
	/// <summary>
	/// The distance of a point from a line made from point1 and point2.
	/// </summary>
	/// <param name="pt1">The line's point1.</param>
	/// <param name="pt2">The line's point2.</param>
	/// <param name="p">The point.</param>
	/// <returns></returns>
	static float PerpendicularDistance(Vector2 point1, Vector2 point2, Vector2 point)
	{
		//Area = |(1/2)(x1y2 + x2y3 + x3y1 - x2y1 - x3y2 - x1y3)|   *Area of triangle
		//Base = v((x1-x2)²+(x1-x2)²)                               *Base of Triangle*
		//Area = .5*Base*H                                          *Solve for height
		//Height = Area/.5/Base

		float area = std::abs(0.5f * (point1.x * point2.y + point2.x * 
			point.y + point.x * point1.y - point2.x * point1.y - point.x * 
			point2.y - point1.x * point.y));
		float bottom = std::sqrt(std::pow(point1.x - point2.x, 2) + 
			std::pow(point1.y - point2.y, 2));
		float height = area / bottom * 2;

		return height;

		//Another option
		//Double A = point.x - point1.x;
		//Double B = point.y - point1.y;
		//Double C = point2.x - point1.x;
		//Double D = point2.y - point1.y;

		//Double dot = A * C + B * D;
		//Double len_sq = C * C + D * D;
		//Double param = dot / len_sq;

		//Double xx, yy;

		//if (param < 0)
		//{
		//    xx = point1.x;
		//    yy = point1.y;
		//}
		//else if (param > 1)
		//{
		//    xx = point2.x;
		//    yy = point2.y;
		//}
		//else
		//{
		//    xx = point1.x + param * C;
		//    yy = point1.y + param * D;
		//}

		//Double d = DistanceBetweenOn2DPlane(point, new point(xx, yy));
	}

	/// <summary> Douglases the peucker reduction. </summary>
	/// <remarks> This is an auxiliary recurse method. </remarks>
	/// <param name="points"> The points. </param>
	/// <param name="firstPoint"> The first point. </param>
	/// <param name="lastPoint"> The last point. </param>
	/// <param name="tolerance"> The tolerance. </param>
	/// <param name="pointIndexsToKeep"> The point index to keep. </param>
	static void _DouglasPeuckerReduction(const std::vector<Vector2> &points, 
		uint firstPoint, uint lastPoint, float tolerance, 
		std::vector<uint> &pointIndexsToKeep)
	{
		float maxDistance = 0;
		uint indexFarthest = 0;

		for (uint index = firstPoint; index < lastPoint; ++index)
		{
			float distance = PerpendicularDistance
				(points[firstPoint], points[lastPoint], points[index]);
			if (distance > maxDistance)
			{
				maxDistance = distance;
				indexFarthest = index;
			}
		}

		if (maxDistance > tolerance && indexFarthest != 0)
		{
			//Add the largest point that exceeds the tolerance
			pointIndexsToKeep.push_back(indexFarthest);

			_DouglasPeuckerReduction(points, firstPoint, 
				indexFarthest, tolerance, pointIndexsToKeep);
			_DouglasPeuckerReduction(points, indexFarthest, 
				lastPoint, tolerance, pointIndexsToKeep);
		}
	}

	/// <summary> Uses the Douglas Peucker algorithm to reduce the number of
	/// 	points. </summary>
	/// <param name="points"> The point path to simplify. </param>
	/// <param name="tolerance"> The tolerance. </param>
	/// <param name="simplifiedPoints"> The vector to old the new simplified
	/// 	point path. </param>
	static void DouglasPeuckerReduction(const std::vector<Vector2> &points, 
		float tolerance, std::vector<Vector2> &simplifiedPoints)
	{
		if (points.size() < 3)
		{
			simplifiedPoints = points;
			return;
		}

		uint firstPoint = 0;
		uint lastPoint = points.size() - 1;
		std::vector<uint> pointIndexsToKeep;

		//Add the first and last index to the keepers
		pointIndexsToKeep.push_back(firstPoint);
		pointIndexsToKeep.push_back(lastPoint);

		//The first and the last point cannot be the same
		while (points[firstPoint] == points[lastPoint])
		{
			lastPoint--;
		}

		_DouglasPeuckerReduction(points, firstPoint, lastPoint, 
			tolerance, pointIndexsToKeep);

		std::sort(pointIndexsToKeep.begin(), pointIndexsToKeep.end());
		uint pointsCnt = pointIndexsToKeep.size();
		for(uint index = 0; index < pointsCnt; ++index)
		{
			simplifiedPoints.push_back(points[pointIndexsToKeep[index]]);
		}

	}
}
#endif // PhySketchUtils_h__
