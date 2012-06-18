#pragma once
#ifndef PhySketchUtils_h__
#define PhySketchUtils_h__


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
}
#endif // PhySketchUtils_h__
