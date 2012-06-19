#pragma once
#ifndef PhySketchMaterial_h__
#define PhySketchMaterial_h__

#include "PhySketchVector2.h"

namespace PhySketch
{
	/// <summary> Represents a RGBA color, with values in the [0,1] range. </summary>
	class Color 
	{
	public:
		Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
		Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

		float r;
		float g;
		float b;
		float a;
	};

	class Material
	{
	public:
		virtual void setColor(Color c) { _color = c; }
		virtual Color getColor() { return _color; }

	protected:
		Color _color;
	};
}
#endif // PhySketchMaterial_h__
