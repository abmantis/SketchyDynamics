#ifndef PhySketchCore_h__
#define PhySketchCore_h__
#pragma once

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"

namespace PhySketch
{
	class Core : public Singleton<Core>
	{
	public:
		Core(void);
		virtual ~Core(void);

		virtual void initialise(std::string logfile, bool logToConsole);	
		
		/// <summary> Creates a window. </summary>
		/// <param name="title"> The title. </param>
		/// <param name="width"> The width. </param>
		/// <param name="height"> The height. </param>
		/// <param name="fullscreen"> true to fullscreen. </param>
		/// <returns> The new window. </returns>
		virtual ApplicationWindow* createWindow(std::string title, int width, int height, bool fullscreen);
		virtual const ApplicationWindow* getWindow() const;
		
		virtual void startLoop();

		static Core* getSingletonPtr(void);
		static Core& getSingleton(void);
	protected:
		ApplicationWindow* _window;
		Logger *_logger;
		Renderer *_renderer;
		MainInputListener *_mainInputListener;

	};
}
#endif // PhySketchCore_h__