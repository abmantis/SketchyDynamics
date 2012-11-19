#pragma once
#ifndef SketchyDynamicsMaterial_h__
#define SketchyDynamicsMaterial_h__

#include "SketchyDynamicsVector2.h"

namespace SketchyDynamics
{
	/// <summary> Represents a RGBA color, with values in the [0,1] range. </summary>
	class Color 
	{
	public:
		Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
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
#endif // SketchyDynamicsMaterial_h__
