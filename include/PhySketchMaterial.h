#pragma once
#ifndef PhySketchMaterial_h__
#define PhySketchMaterial_h__

#include "PhySketchVector2.h"

namespace PhySketch
{
	/// <summary> Represents a RGBA color, with values in the [0,1] range. </summary>
	class Color 
	{
		Color() : r(0), g(0), b(0), a(0) {}

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
