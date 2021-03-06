#ifndef SketchyDynamicsCore_h__
#define SketchyDynamicsCore_h__
#pragma once

#include "SketchyDynamicsDefinitions.h"
#include "SketchyDynamicsSingleton.h"

namespace SketchyDynamics
{
	class Core : public Singleton<Core>
	{
	public:
		Core(void);
		virtual ~Core(void);

		virtual void initialise(std::string logfile, bool logToConsole, Vector2 physicsGravity, Vector2 physicsWorldSize);	
		
		/// <summary> Creates a window. </summary>
		/// <param name="title"> The title. </param>
		/// <param name="width"> The width. </param>
		/// <param name="height"> The height. </param>
		/// <param name="fullscreen"> true to fullscreen. </param>
		/// <returns> The new window. </returns>
		virtual ApplicationWindow* createWindow(std::string title, Vector2 position, Vector2 size, bool fullscreen, const ParameterMap *parameters = nullptr);

		/// <summary> Gets the window. </summary>		
		/// <returns> The window, or NULL if createWindow() was not called. </returns>
		virtual ApplicationWindow* getWindow() const;

		virtual void startLoop();
		
		virtual bool doOneFrame();

		static Core* getSingletonPtr(void);
		static Core& getSingleton(void);

	protected:
		virtual inline bool _doOneFrame(ulong timeSinceLastFrame);

	protected:
		ApplicationWindow* _window;
		Logger *_logger;
		Renderer *_renderer;
		MaterialManager* _materialManager;
		MainInputListener *_mainInputListener;
		PhysicsManager *_physicsMgr;
		clock_t _currentFrame, _lastFrame;
	};
}
#endif // SketchyDynamicsCore_h__