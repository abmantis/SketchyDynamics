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
	
	static std::string toString(unsigned int i)
	{
		std::ostringstream stream;
		stream << i;
		return stream.str();
	}

	static float stringToFloat(const char* str)
	{
		std::istringstream stm;
		stm.str(str);
		float f;
		stm >> f;
		return f;
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

	/// <summary> Find the line-line intersection point. </summary>
	/// <param name="start1"> The line 1 start point. </param>
	/// <param name="end1"> The line 1 end point. </param>
	/// <param name="start2"> The line 2 start point. </param>
	/// <param name="end2"> The line 2 end point. </param>
	/// <param name="intersection"> [out] The intersection (if it exists). </param>
	/// <returns> True if an intersection point exists. </returns>
	static bool lineLineIntersection(Vector2 start1, Vector2 end1, Vector2 start2, Vector2 end2, Vector2 &intersection) 
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


	static bool isCounterClockwise(Vector2 p1, Vector2 p2, Vector2 p3)
	{
		return ((p2.y-p1.y)*(p3.x-p2.x)<(p3.y-p2.y)*(p2.x-p1.x));
	}
}
#endif // PhySketchUtils_h__
