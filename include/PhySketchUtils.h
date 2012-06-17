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
}
#endif // PhySketchUtils_h__
