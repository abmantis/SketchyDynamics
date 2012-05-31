#pragma once
#ifndef PhySketchUtils_h__
#define PhySketchUtils_h__


namespace PhySketch
{
	static std::string toString(double d)
	{
		std::ostringstream stream;
		stream << d;
		return stream.str();
	}

	static double stringToDouble(const char* str)
	{
		std::istringstream stm;
		stm.str(str);
		double d;
		stm >> d;
		return d;
	}

	static double degreesToRadians (double degrees)
	{
		double radians = (M_PI / 180.0f) * degrees;
		return (radians);
	}

	static double radiansToDegrees (double radians)
	{
		double degrees = (180.0f / M_PI) * radians;
		return (degrees);
	}

	static long round(double d)
	{
		return (long)floor(d + 0.5);
	}
}
#endif // PhySketchUtils_h__
