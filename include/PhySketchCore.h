#ifndef PhySketchCore_h__
#define PhySketchCore_h__
#pragma once

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"
#include "Box2D/Box2D.h"

namespace PhySketch
{
	class Core : public Singleton<Core>
	{
	public:
		Core(void);
		virtual ~Core(void);

		virtual void initialise(std::string logfile, bool logToConsole,  Vector2 physicsGravity);	
		
		/// <summary> Creates a window. </summary>
		/// <param name="title"> The title. </param>
		/// <param name="width"> The width. </param>
		/// <param name="height"> The height. </param>
		/// <param name="fullscreen"> true to fullscreen. </param>
		/// <returns> The new window. </returns>
		virtual ApplicationWindow* createWindow(std::string title, int width, int height, bool fullscreen);

		/// <summary> Gets the window. </summary>		
		/// <returns> The window, or NULL if createWindow() was not called. </returns>
		virtual ApplicationWindow* getWindow() const;

		virtual b2World* createPhysicsWorld(Vector2 gravity);

		/// <summary> Gets the physics world. </summary>
		/// <returns> The physics world. </returns>
		virtual b2World* getPhysicsWorld() const;
		
		virtual void startLoop();

		static Core* getSingletonPtr(void);
		static Core& getSingleton(void);

	protected:
		virtual void stepPhysics(ulong ellapsedMillisec);

	protected:
		ApplicationWindow* _window;
		Logger *_logger;
		Renderer *_renderer;
		MainInputListener *_mainInputListener;
		PhysicsManager *_physicsMgr;
		b2World *_physicsWorld;

	};
}
#endif // PhySketchCore_h__