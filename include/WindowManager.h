#ifndef WindowManager_h__
#define WindowManager_h__
#pragma once

namespace PhySketch
{
	class WindowManager
	{
	public:
		WindowManager(void);
		~WindowManager(void);


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
		bool CreateWindow(std::string title, int width, int height, bool fullscreen);

	private:
		bool		_fullscreen;	// Fullscreen flag
		HDC			_hDC;			// Private GDI Device Context
		HGLRC		_hRC;			// Permanent Rendering Context
		HWND		_hWnd;			// Holds Our Window Handle
		HINSTANCE	_hInstance;		// Holds The Instance Of The Application
	};

}// namespace PhySketch

#endif // WindowManager_h__
