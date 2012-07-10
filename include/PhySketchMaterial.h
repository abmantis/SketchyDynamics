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
		Color() : r(1.0f), g(1.0f), b(1.0f), a(0.0f) {}
		Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

		float r;
		float g;
		float b;
		float a;
	};

	class Material
	{
		friend class MaterialManager;
	protected:
		Material(std::string name) : 
			 _name		(name),
			_textureID	(0)
		{
		}		

	public:
		

		virtual void setColor(const Color& c) { _color = c; }
		virtual const Color getColor() const { return _color; }

		virtual const uint getTextureID() const { return _textureID; }		

	protected:
		std::string _name;
		Color _color;
		
		uint _textureID;
	};
}
#endif // PhySketchMaterial_h__
