#ifndef WindowManager_h__
#define WindowManager_h__
#pragma once

#include "PhySketchDefinitions.h"
#include "PhySketchInputListener.h"

#define PHYSKETCH_WINDOW_CLASS_NAME "PhySketchAppWindow"

namespace PhySketch
{
	class ApplicationWindow
	{
	public:
		ApplicationWindow(void);
		virtual ~ApplicationWindow(void);


		/**
		 * <summary>	Creates a window. </summary>
		 *
		 * <param name="title">	Window title. </param>
		 * <param name="width">	The width. </param>
		 * <param name="height">	The height. </param>
		 * <param name="fullscreen">	true to make fullscreen window. </param>
		 *
		 * <returns>	true if window was created successfully. </returns>
		 */
		virtual bool createWindow(std::string title, int width, int height, bool fullscreen) = 0;

		/**
		 * <summary> Properly destroy the window </summary>
		 */
		virtual void destroyWindow(void) = 0;

		/**
		 * <summary> Update the window and process any messages. </summary>
		 *
		 * <returns> false if the window received a close message </returns>
		 */
		virtual bool updateWindow() = 0;

		virtual bool isClosePending() { return _closePending; }

		virtual void addInputListener(InputListener * inputListener);
		virtual void removeInputListener(InputListener * inputListener);


		/// <summary> Turns VSync on/off. </summary>
		/// <param name="on"> true to turn VSync on. </param>
		virtual void setVSync(bool on) = 0;

	protected:
		virtual void updateInputListeners();
		virtual void callInputListenersKeyDown(Key key);
		virtual void callInputListenersKeyUp(Key key);
		virtual void callInputListenersMouseDown(MouseButton button, Vector2 position);
		virtual void callInputListenersMouseUp(MouseButton button, Vector2 position);
		virtual void callInputListenersMouseMoved(Vector2 position);

	protected:
		bool _closePending;	// is the window waiting to be closed?
		bool _active;		// is the window active (not minimized)?
		bool _fullscreen;	// fullscreen window?
		std::set<InputListener*> _inputListeners;			// input listeners that wold be called
		std::set<InputListener*> _inputListenersToRemove;	// input listeners that should be removed from the IL list
		Renderer *_renderer;
		double _aspectRatio;
	};

}// namespace PhySketch

#endif // WindowManager_h__
