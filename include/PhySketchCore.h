#ifndef PhySketchCore_h__
#define PhySketchCore_h__
#pragma once

#include "PhySketchDefinitions.h"

namespace PhySketch
{
	class Core
	{
	public:
		Core(void);
		virtual ~Core(void);

		virtual ApplicationWindow* createWindow(std::string title, int width, int height, bool fullscreen);
		
		virtual void startLoop();

	protected:
		ApplicationWindow* _window;
	};
}
#endif // PhySketchCore_h__