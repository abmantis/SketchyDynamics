#ifndef WindowManager_h__
#define WindowManager_h__
#pragma once

#include "PhySketchDefinitions.h"

namespace PhySketch
{
	class ApplicationWindow
	{
	public:
		ApplicationWindow(void) { _fullscreen = false; }
		virtual ~ApplicationWindow(void) {};


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

	protected:
		bool		_fullscreen;	// Fullscreen flag
	};



	class ApplicationWindow_WGL : public ApplicationWindow
	{
	public:
		ApplicationWindow_WGL(void);
		virtual ~ApplicationWindow_WGL(void);


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
		virtual bool createWindow(std::string title, int width, int height, bool fullscreen);

		/**
		 * <summary> Properly destroy the window </summary>
		 */
		virtual void destroyWindow(void);

	protected:

		/**
		 * <summary> The actual windows procedure that gets called after
		 *		the WM_NCCREATE message is processed. This is a member
		 *		function so it can access member variables and functions. </summary>
		 */
		virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
		
		/**
		 * <summary> The initial WndProc assigned to WNDCLASS. This WndProc waits for the 
		 * 			 WM_NCCREATE message and then sets the other WndProc (staticWndProc).
		 * 			 This is needed because when the window is created and WM_NCCREATE
		 * 			 is received there's no userdata 
		 * 			 (http://members.gamedev.net/sicrane/articles/WindowClass.html) </summary>
		 */
		static LRESULT CALLBACK initialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

		/**
		 * <summary> The seccond (and final) WndProc assigned to WNDCLASS.
		 * 			 This WndProc calls the member function from ApplicationWindow_WGL </summary>
		 */
		static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	
		/**
		 * <summary> Resize And Initialize The GL Window </summary>
		 */		
		virtual void resizeGLScene(int width, int height);

		/**
		 * <summary> Setup OpenGL </summary>
		 */		
		virtual int initGL(void);

	protected:
		HDC			_hDC;			// Private GDI Device Context
		HGLRC		_hRC;			// Permanent Rendering Context
		HWND		_hWnd;			// Holds Our Window Handle
		HINSTANCE	_hInstance;		// Holds The Instance Of The Application
	};

}// namespace PhySketch

#endif // WindowManager_h__
