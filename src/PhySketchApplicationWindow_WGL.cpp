
#include "PhySketchApplicationWindow_WGL.h"
#include "PhySketchLogger.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "PhySketchVector2.h"

namespace PhySketch
{
	ApplicationWindow_WGL::ApplicationWindow_WGL(void) : ApplicationWindow()
	{
		_hDC		= NULL;
		_hRC		= NULL;		
		_hWnd		= NULL;		
		_hInstance	= NULL;	
	}

	ApplicationWindow_WGL::~ApplicationWindow_WGL(void)
	{
		destroyWindow();
	}

	bool ApplicationWindow_WGL::createWindow( std::string title, int width, int height, bool fullscreen)
	{
		int bpp = 32;
		int			PixelFormat;			// Holds The Results After Searching For A Match
		WNDCLASS	wc;						// Windows Class Structure
		DWORD		dwExStyle;				// Window Extended Style
		DWORD		dwStyle;				// Window Style
		RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left	= (long)0;			// Set Left Value To 0
		WindowRect.right = (long)width;		// Set Right Value To Requested Width
		WindowRect.top = (long)0;			// Set Top Value To 0
		WindowRect.bottom = (long)height;	// Set Bottom Value To Requested Height

		_fullscreen = fullscreen;			

		_hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window

		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc		= &ApplicationWindow_WGL::initialWndProc;// WndProc Handles Messages
		wc.cbClsExtra		= 0;									// No Extra Window Data
		wc.cbWndExtra		= 0;									// No Extra Window Data
		wc.hInstance		= _hInstance;							// Set The Instance
		wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground	= NULL;									// No Background Required For GL
		wc.lpszMenuName		= NULL;									// We Don't Want A Menu
		wc.lpszClassName	= PHYSKETCH_WINDOW_CLASS_NAME;			// Set The Class Name

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		{
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Failed To Register The Window Class");
			return FALSE;											// Return FALSE
		}

		if (fullscreen)												// Attempt Fullscreen Mode?
		{
			DEVMODE dmScreenSettings;								// Device Mode
			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
			dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
			dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
			dmScreenSettings.dmBitsPerPel	= bpp;					// Selected Bits Per Pixel
			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			{
				// If The Mode Fails, Use Windowed Mode.			
				fullscreen=FALSE;
				Logger::getSingletonPtr()->writeWarning("Fullscreen mode not supported");			
			}
		}

		if (fullscreen)												// Are We Still In Fullscreen Mode?
		{
			dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
			dwStyle=WS_POPUP;										// Windows Style
			ShowCursor(FALSE);										// Hide Mouse Pointer
		}
		else
		{
			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

		// Create The Window
		if (!(_hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
			PHYSKETCH_WINDOW_CLASS_NAME,		// Class Name
			title.c_str(),						// Window Title
			dwStyle |							// Defined Window Style
			WS_CLIPSIBLINGS |					// Required Window Style
			WS_CLIPCHILDREN,					// Required Window Style
			0, 0,								// Window Position
			WindowRect.right-WindowRect.left,	// Calculate Window Width
			WindowRect.bottom-WindowRect.top,	// Calculate Window Height
			NULL,								// No Parent Window
			NULL,								// No Menu
			_hInstance,							// Instance
			this)))								// Dont Pass Anything To WM_CREATE
		{
			destroyWindow();								// Reset The Display
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Window Creation Error");
			return FALSE;								// Return FALSE
		}

		static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			bpp,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		if (!(_hDC=GetDC(_hWnd)))							// Did We Get A Device Context?
		{
			destroyWindow();								// Reset The Display
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Can't Create A GL Device Context");
			return FALSE;								// Return FALSE
		}

		if (!(PixelFormat=ChoosePixelFormat(_hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			destroyWindow();								// Reset The Display
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Can't Find A Suitable PixelFormat");
			return FALSE;								// Return FALSE
		}

		if(!SetPixelFormat(_hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
		{
			destroyWindow();								// Reset The Display
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Can't Set The PixelFormat");
			return FALSE;								// Return FALSE
		}

		if (!(_hRC=wglCreateContext(_hDC)))				// Are We Able To Get A Rendering Context?
		{
			destroyWindow();								// Reset The Display
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Can't Create A GL Rendering Context");
			return FALSE;								// Return FALSE
		}

		if(!wglMakeCurrent(_hDC,_hRC))					// Try To Activate The Rendering Context
		{
			destroyWindow();								// Reset The Display
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Can't Activate The GL Rendering Context");
			return FALSE;								// Return FALSE
		}

		ShowWindow(_hWnd, SW_SHOW);						// Show The Window
		SetForegroundWindow(_hWnd);						// Slightly Higher Priority
		SetFocus(_hWnd);								// Sets Keyboard Focus To The Window
		resizeGLScene(width, height);					// Set Up Our Perspective GL Screen

		if (!initGL())									// Initialize Our Newly Created GL Window
		{
			destroyWindow();					// Reset The Display
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Initialization Failed.");
			return FALSE;								// Return FALSE
		}

		return TRUE;									// Success
	}

	void ApplicationWindow_WGL::destroyWindow(void)								
	{
		if (_fullscreen)									// Are We In Fullscreen Mode?
		{
			ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
			ShowCursor(TRUE);								// Show Mouse Pointer
		}

		if (_hRC)											// Do We Have A Rendering Context?
		{
			if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
			{
				Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Release Of DC And RC Failed");
			}

			if (!wglDeleteContext(_hRC))					// Are We Able To Delete The RC?
			{
				Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Release Rendering Context Failed");
			}
			_hRC=NULL;										// Set RC To NULL
		}

		if (_hDC && !ReleaseDC(_hWnd, _hDC))				// Are We Able To Release The DC
		{
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Release Device Context Failed");
			_hDC=NULL;										// Set DC To NULL
		}

		if (_hWnd && !DestroyWindow(_hWnd))					// Are We Able To Destroy The Window?
		{
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Could Not Release hWnd");
			_hWnd=NULL;										// Set hWnd To NULL
		}

		if (!UnregisterClass(PHYSKETCH_WINDOW_CLASS_NAME, _hInstance))			// Are We Able To Unregister Class
		{
			Logger::getSingletonPtr()->writeError("{ApplicationWindow_WGL}Could Not Unregister Class");
			_hInstance=NULL;									// Set hInstance To NULL
		}
	}

	int ApplicationWindow_WGL::initGL( void )
	{
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
		//glClearDepth(1.0f);									// Depth Buffer Setup
		//glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		//glDepthFunc(GL_LEQUAL);							// The Type Of Depth Testing To Do
		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing because we are in 2D	
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		return TRUE;										// Initialization Went OK
	}

	bool ApplicationWindow_WGL::updateWindow()
	{
		MSG	msg;
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				_closePending = true;
				return false;
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}

		if(drawGLScene())
		{
			SwapBuffers(_hDC);
		}
		else
		{	
			_closePending = true;
			return false;
		}

		return true;
	}

	void ApplicationWindow_WGL::resizeGLScene(int width, int height)		// Resize And Initialize The GL Window
	{
		glViewport(0,0,width,height);						// Reset The Current Viewport

		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix

		double ratio = double(width) / double(height);

		Vector2 extents(6.0f, 6.0f / ratio);

		// Using Ortho for 2D
		gluOrtho2D(-extents.x, extents.x, -extents.y, extents.y);
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glLoadIdentity();									// Reset The Modelview Matrix

	}

	LRESULT CALLBACK ApplicationWindow_WGL::initialWndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
	{
		if (Msg == WM_NCCREATE) {
			LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			void * lpCreateParam = create_struct->lpCreateParams;
			ApplicationWindow_WGL * this_window = reinterpret_cast<ApplicationWindow_WGL *>(lpCreateParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this_window));
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&ApplicationWindow_WGL::staticWndProc));
			return this_window->WndProc(hWnd, Msg, wParam, lParam);
		}
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	LRESULT CALLBACK ApplicationWindow_WGL::staticWndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
	{
		LONG_PTR user_data = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		ApplicationWindow_WGL * this_window = reinterpret_cast<ApplicationWindow_WGL *>(user_data);
		return this_window->WndProc(hWnd, Msg, wParam, lParam);
	}

	LRESULT CALLBACK ApplicationWindow_WGL::WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
	{
		//	std::cout << "WndProc_MESSAGE" << std::endl;

		if(!_inputListenersToRemove.empty())
		{
			// remove InputListeners before doing anything else
			updateInputListeners();
		}

		switch (Msg) 
		{
		case WM_ACTIVATE:							
			{
				if (!HIWORD(wParam))					// Check Minimization State
				{
					_active = TRUE;						// Program Is Active
				}
				else
				{
					_active = FALSE;					// Program Is No Longer Active
				}
				break;
			}
		case WM_SIZE:
			{
				resizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
				break;
			}
		case WM_CLOSE:
			{
				PostQuitMessage(0);
				break;
			}
			//case WM_QUIT:
			//{
			//	_closePending = true;
			//	break;
			//} 
		case WM_LBUTTONDOWN:
			{				
				callInputListenersMouseDown( MB_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
				break;
			}
		case WM_LBUTTONUP:
			{				
				callInputListenersMouseUp( MB_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
				break;
			}
		case WM_MBUTTONDOWN:
			{				
				callInputListenersMouseDown( MB_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
				break;
			}
		case WM_MBUTTONUP:
			{				
				callInputListenersMouseUp( MB_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
				break;
			}
		case WM_RBUTTONDOWN:
			{				
				callInputListenersMouseDown( MB_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
				break;
			}
		case WM_RBUTTONUP:
			{				
				callInputListenersMouseUp( MB_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
				break;
			}
		case WM_MOUSEMOVE:
			{				
				callInputListenersMouseMoved( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
				break;
			}

#pragma region Keyboard events
		case WM_KEYDOWN:
			{
				switch(wParam)
				{
				case VK_BACK:		callInputListenersKeyDown(KEY_Back); break;
				case VK_TAB:		callInputListenersKeyDown(KEY_Tab); break;
				case VK_CLEAR:		callInputListenersKeyDown(KEY_Clear); break;
				case VK_RETURN:		callInputListenersKeyDown(KEY_Return); break;
				case VK_SHIFT:		callInputListenersKeyDown(KEY_Shift); break;
				case VK_CONTROL:	callInputListenersKeyDown(KEY_Control); break;
				case VK_MENU:		callInputListenersKeyDown(KEY_Menu); break;
				case VK_PAUSE:		callInputListenersKeyDown(KEY_Pause); break;
				case VK_CAPITAL:	callInputListenersKeyDown(KEY_Capital); break;
				case VK_KANA:		callInputListenersKeyDown(KEY_Kana); break;
				case VK_JUNJA:		callInputListenersKeyDown(KEY_Junja); break;
				case VK_FINAL:		callInputListenersKeyDown(KEY_Final); break;
				case VK_HANJA:		callInputListenersKeyDown(KEY_Hanja); break;
				case VK_ESCAPE:		callInputListenersKeyDown(KEY_Escape); break;
				case VK_CONVERT:	callInputListenersKeyDown(KEY_Convert); break;
				case VK_NONCONVERT:	callInputListenersKeyDown(KEY_Nonconvert); break;
				case VK_ACCEPT:		callInputListenersKeyDown(KEY_Accept); break;
				case VK_MODECHANGE:	callInputListenersKeyDown(KEY_Modechange); break;
				case VK_SPACE:		callInputListenersKeyDown(KEY_Space); break;
				case VK_PRIOR:		callInputListenersKeyDown(KEY_Prior); break;
				case VK_NEXT:		callInputListenersKeyDown(KEY_Next); break;
				case VK_END:		callInputListenersKeyDown(KEY_End); break;
				case VK_HOME:		callInputListenersKeyDown(KEY_Home); break;
				case VK_LEFT:		callInputListenersKeyDown(KEY_Left); break;
				case VK_UP:			callInputListenersKeyDown(KEY_Up); break;
				case VK_RIGHT:		callInputListenersKeyDown(KEY_Right); break;
				case VK_DOWN:		callInputListenersKeyDown(KEY_Down); break;
				case VK_SELECT:		callInputListenersKeyDown(KEY_Select); break;
				case VK_PRINT:		callInputListenersKeyDown(KEY_Print); break;
				case VK_EXECUTE:	callInputListenersKeyDown(KEY_Execute); break;
				case VK_SNAPSHOT:	callInputListenersKeyDown(KEY_Snapshot); break;
				case VK_INSERT:		callInputListenersKeyDown(KEY_Insert); break;
				case VK_DELETE:		callInputListenersKeyDown(KEY_Delete); break;
				case VK_HELP:		callInputListenersKeyDown(KEY_Help); break;
				case VK_LWIN:		callInputListenersKeyDown(KEY_Lwin); break;
				case VK_RWIN:		callInputListenersKeyDown(KEY_Rwin); break;
				case VK_APPS:		callInputListenersKeyDown(KEY_Apps); break;
				case VK_SLEEP:		callInputListenersKeyDown(KEY_Sleep); break;
				case VK_NUMPAD0:	callInputListenersKeyDown(KEY_Numpad0); break;
				case VK_NUMPAD1:	callInputListenersKeyDown(KEY_Numpad1); break;
				case VK_NUMPAD2:	callInputListenersKeyDown(KEY_Numpad2); break;
				case VK_NUMPAD3:	callInputListenersKeyDown(KEY_Numpad3); break;
				case VK_NUMPAD4:	callInputListenersKeyDown(KEY_Numpad4); break;
				case VK_NUMPAD5:	callInputListenersKeyDown(KEY_Numpad5); break;
				case VK_NUMPAD6:	callInputListenersKeyDown(KEY_Numpad6); break;
				case VK_NUMPAD7:	callInputListenersKeyDown(KEY_Numpad7); break;
				case VK_NUMPAD8:	callInputListenersKeyDown(KEY_Numpad8); break;
				case VK_NUMPAD9:	callInputListenersKeyDown(KEY_Numpad9); break;
				case VK_MULTIPLY:	callInputListenersKeyDown(KEY_Multiply); break;
				case VK_ADD:		callInputListenersKeyDown(KEY_Add); break;
				case VK_SEPARATOR:	callInputListenersKeyDown(KEY_Separator); break;
				case VK_SUBTRACT:	callInputListenersKeyDown(KEY_Subtract); break;
				case VK_DECIMAL:	callInputListenersKeyDown(KEY_Decimal); break;
				case VK_DIVIDE:		callInputListenersKeyDown(KEY_Divide); break;
				case VK_F1:			callInputListenersKeyDown(KEY_F1); break;
				case VK_F2:			callInputListenersKeyDown(KEY_F2); break;
				case VK_F3:			callInputListenersKeyDown(KEY_F3); break;
				case VK_F4:			callInputListenersKeyDown(KEY_F4); break;
				case VK_F5:			callInputListenersKeyDown(KEY_F5); break;
				case VK_F6:			callInputListenersKeyDown(KEY_F6); break;
				case VK_F7:			callInputListenersKeyDown(KEY_F7); break;
				case VK_F8:			callInputListenersKeyDown(KEY_F8); break;
				case VK_F9:			callInputListenersKeyDown(KEY_F9); break;
				case VK_F10:		callInputListenersKeyDown(KEY_F10); break;
				case VK_F11:		callInputListenersKeyDown(KEY_F11); break;
				case VK_F12:		callInputListenersKeyDown(KEY_F12); break;
				case VK_F13:		callInputListenersKeyDown(KEY_F13); break;
				case VK_F14:		callInputListenersKeyDown(KEY_F14); break;
				case VK_F15:		callInputListenersKeyDown(KEY_F15); break;
				case VK_F16:		callInputListenersKeyDown(KEY_F16); break;
				case VK_F17:		callInputListenersKeyDown(KEY_F17); break;
				case VK_F18:		callInputListenersKeyDown(KEY_F18); break;
				case VK_F19:		callInputListenersKeyDown(KEY_F19); break;
				case VK_F20:		callInputListenersKeyDown(KEY_F20); break;
				case VK_F21:		callInputListenersKeyDown(KEY_F21); break;
				case VK_F22:		callInputListenersKeyDown(KEY_F22); break;
				case VK_F23:		callInputListenersKeyDown(KEY_F23); break;
				case VK_F24:		callInputListenersKeyDown(KEY_F24); break;
				case VK_NUMLOCK:	callInputListenersKeyDown(KEY_Numlock); break;
				case VK_SCROLL:		callInputListenersKeyDown(KEY_Scrolllock); break;
				case VK_OEM_PLUS:	callInputListenersKeyDown(KEY_OEM_Plus); break;
				case VK_OEM_COMMA:	callInputListenersKeyDown(KEY_OEM_Comma); break;
				case VK_OEM_MINUS:	callInputListenersKeyDown(KEY_OEM_Minus); break;
				case VK_OEM_PERIOD:	callInputListenersKeyDown(KEY_OEM_Period); break;
				case 'Q':			callInputListenersKeyDown(KEY_Q); break;
				case 'W':			callInputListenersKeyDown(KEY_W); break;
				case 'E':			callInputListenersKeyDown(KEY_E); break;
				case 'R':			callInputListenersKeyDown(KEY_R); break;
				case 'T':			callInputListenersKeyDown(KEY_T); break;
				case 'Y':			callInputListenersKeyDown(KEY_Y); break;
				case 'U':			callInputListenersKeyDown(KEY_U); break;
				case 'I':			callInputListenersKeyDown(KEY_I); break;
				case 'O':			callInputListenersKeyDown(KEY_O); break;
				case 'P':			callInputListenersKeyDown(KEY_P); break;
				case 'A':			callInputListenersKeyDown(KEY_A); break;
				case 'S':			callInputListenersKeyDown(KEY_S); break;
				case 'D':			callInputListenersKeyDown(KEY_D); break;
				case 'F':			callInputListenersKeyDown(KEY_F); break;
				case 'G':			callInputListenersKeyDown(KEY_G); break;
				case 'H':			callInputListenersKeyDown(KEY_H); break;
				case 'J':			callInputListenersKeyDown(KEY_J); break;
				case 'K':			callInputListenersKeyDown(KEY_K); break;
				case 'L':			callInputListenersKeyDown(KEY_L); break;
				case 'Z':			callInputListenersKeyDown(KEY_Z); break;
				case 'X':			callInputListenersKeyDown(KEY_X); break;
				case 'C':			callInputListenersKeyDown(KEY_C); break;
				case 'V':			callInputListenersKeyDown(KEY_V); break;
				case 'B':			callInputListenersKeyDown(KEY_B); break;
				case 'N':			callInputListenersKeyDown(KEY_N); break;
				case 'M':			callInputListenersKeyDown(KEY_M); break;
				default:
					break;
				}
			}
		case WM_KEYUP:
			{
				switch(wParam)
				{
				case VK_BACK:		callInputListenersKeyUp(KEY_Back); break;
				case VK_TAB:		callInputListenersKeyUp(KEY_Tab); break;
				case VK_CLEAR:		callInputListenersKeyUp(KEY_Clear); break;
				case VK_RETURN:		callInputListenersKeyUp(KEY_Return); break;
				case VK_SHIFT:		callInputListenersKeyUp(KEY_Shift); break;
				case VK_CONTROL:	callInputListenersKeyUp(KEY_Control); break;
				case VK_MENU:		callInputListenersKeyUp(KEY_Menu); break;
				case VK_PAUSE:		callInputListenersKeyUp(KEY_Pause); break;
				case VK_CAPITAL:	callInputListenersKeyUp(KEY_Capital); break;
				case VK_KANA:		callInputListenersKeyUp(KEY_Kana); break;
				case VK_JUNJA:		callInputListenersKeyUp(KEY_Junja); break;
				case VK_FINAL:		callInputListenersKeyUp(KEY_Final); break;
				case VK_HANJA:		callInputListenersKeyUp(KEY_Hanja); break;
				case VK_ESCAPE:		callInputListenersKeyUp(KEY_Escape); break;
				case VK_CONVERT:	callInputListenersKeyUp(KEY_Convert); break;
				case VK_NONCONVERT:	callInputListenersKeyUp(KEY_Nonconvert); break;
				case VK_ACCEPT:		callInputListenersKeyUp(KEY_Accept); break;
				case VK_MODECHANGE:	callInputListenersKeyUp(KEY_Modechange); break;
				case VK_SPACE:		callInputListenersKeyUp(KEY_Space); break;
				case VK_PRIOR:		callInputListenersKeyUp(KEY_Prior); break;
				case VK_NEXT:		callInputListenersKeyUp(KEY_Next); break;
				case VK_END:		callInputListenersKeyUp(KEY_End); break;
				case VK_HOME:		callInputListenersKeyUp(KEY_Home); break;
				case VK_LEFT:		callInputListenersKeyUp(KEY_Left); break;
				case VK_UP:			callInputListenersKeyUp(KEY_Up); break;
				case VK_RIGHT:		callInputListenersKeyUp(KEY_Right); break;
				case VK_DOWN:		callInputListenersKeyUp(KEY_Down); break;
				case VK_SELECT:		callInputListenersKeyUp(KEY_Select); break;
				case VK_PRINT:		callInputListenersKeyUp(KEY_Print); break;
				case VK_EXECUTE:	callInputListenersKeyUp(KEY_Execute); break;
				case VK_SNAPSHOT:	callInputListenersKeyUp(KEY_Snapshot); break;
				case VK_INSERT:		callInputListenersKeyUp(KEY_Insert); break;
				case VK_DELETE:		callInputListenersKeyUp(KEY_Delete); break;
				case VK_HELP:		callInputListenersKeyUp(KEY_Help); break;
				case VK_LWIN:		callInputListenersKeyUp(KEY_Lwin); break;
				case VK_RWIN:		callInputListenersKeyUp(KEY_Rwin); break;
				case VK_APPS:		callInputListenersKeyUp(KEY_Apps); break;
				case VK_SLEEP:		callInputListenersKeyUp(KEY_Sleep); break;
				case VK_NUMPAD0:	callInputListenersKeyUp(KEY_Numpad0); break;
				case VK_NUMPAD1:	callInputListenersKeyUp(KEY_Numpad1); break;
				case VK_NUMPAD2:	callInputListenersKeyUp(KEY_Numpad2); break;
				case VK_NUMPAD3:	callInputListenersKeyUp(KEY_Numpad3); break;
				case VK_NUMPAD4:	callInputListenersKeyUp(KEY_Numpad4); break;
				case VK_NUMPAD5:	callInputListenersKeyUp(KEY_Numpad5); break;
				case VK_NUMPAD6:	callInputListenersKeyUp(KEY_Numpad6); break;
				case VK_NUMPAD7:	callInputListenersKeyUp(KEY_Numpad7); break;
				case VK_NUMPAD8:	callInputListenersKeyUp(KEY_Numpad8); break;
				case VK_NUMPAD9:	callInputListenersKeyUp(KEY_Numpad9); break;
				case VK_MULTIPLY:	callInputListenersKeyUp(KEY_Multiply); break;
				case VK_ADD:		callInputListenersKeyUp(KEY_Add); break;
				case VK_SEPARATOR:	callInputListenersKeyUp(KEY_Separator); break;
				case VK_SUBTRACT:	callInputListenersKeyUp(KEY_Subtract); break;
				case VK_DECIMAL:	callInputListenersKeyUp(KEY_Decimal); break;
				case VK_DIVIDE:		callInputListenersKeyUp(KEY_Divide); break;
				case VK_F1:			callInputListenersKeyUp(KEY_F1); break;
				case VK_F2:			callInputListenersKeyUp(KEY_F2); break;
				case VK_F3:			callInputListenersKeyUp(KEY_F3); break;
				case VK_F4:			callInputListenersKeyUp(KEY_F4); break;
				case VK_F5:			callInputListenersKeyUp(KEY_F5); break;
				case VK_F6:			callInputListenersKeyUp(KEY_F6); break;
				case VK_F7:			callInputListenersKeyUp(KEY_F7); break;
				case VK_F8:			callInputListenersKeyUp(KEY_F8); break;
				case VK_F9:			callInputListenersKeyUp(KEY_F9); break;
				case VK_F10:		callInputListenersKeyUp(KEY_F10); break;
				case VK_F11:		callInputListenersKeyUp(KEY_F11); break;
				case VK_F12:		callInputListenersKeyUp(KEY_F12); break;
				case VK_F13:		callInputListenersKeyUp(KEY_F13); break;
				case VK_F14:		callInputListenersKeyUp(KEY_F14); break;
				case VK_F15:		callInputListenersKeyUp(KEY_F15); break;
				case VK_F16:		callInputListenersKeyUp(KEY_F16); break;
				case VK_F17:		callInputListenersKeyUp(KEY_F17); break;
				case VK_F18:		callInputListenersKeyUp(KEY_F18); break;
				case VK_F19:		callInputListenersKeyUp(KEY_F19); break;
				case VK_F20:		callInputListenersKeyUp(KEY_F20); break;
				case VK_F21:		callInputListenersKeyUp(KEY_F21); break;
				case VK_F22:		callInputListenersKeyUp(KEY_F22); break;
				case VK_F23:		callInputListenersKeyUp(KEY_F23); break;
				case VK_F24:		callInputListenersKeyUp(KEY_F24); break;
				case VK_NUMLOCK:	callInputListenersKeyUp(KEY_Numlock); break;
				case VK_SCROLL:		callInputListenersKeyUp(KEY_Scrolllock); break;
				case VK_OEM_PLUS:	callInputListenersKeyUp(KEY_OEM_Plus); break;
				case VK_OEM_COMMA:	callInputListenersKeyUp(KEY_OEM_Comma); break;
				case VK_OEM_MINUS:	callInputListenersKeyUp(KEY_OEM_Minus); break;
				case VK_OEM_PERIOD:	callInputListenersKeyUp(KEY_OEM_Period); break;
				case 'Q':			callInputListenersKeyUp(KEY_Q); break;
				case 'W':			callInputListenersKeyUp(KEY_W); break;
				case 'E':			callInputListenersKeyUp(KEY_E); break;
				case 'R':			callInputListenersKeyUp(KEY_R); break;
				case 'T':			callInputListenersKeyUp(KEY_T); break;
				case 'Y':			callInputListenersKeyUp(KEY_Y); break;
				case 'U':			callInputListenersKeyUp(KEY_U); break;
				case 'I':			callInputListenersKeyUp(KEY_I); break;
				case 'O':			callInputListenersKeyUp(KEY_O); break;
				case 'P':			callInputListenersKeyUp(KEY_P); break;
				case 'A':			callInputListenersKeyUp(KEY_A); break;
				case 'S':			callInputListenersKeyUp(KEY_S); break;
				case 'D':			callInputListenersKeyUp(KEY_D); break;
				case 'F':			callInputListenersKeyUp(KEY_F); break;
				case 'G':			callInputListenersKeyUp(KEY_G); break;
				case 'H':			callInputListenersKeyUp(KEY_H); break;
				case 'J':			callInputListenersKeyUp(KEY_J); break;
				case 'K':			callInputListenersKeyUp(KEY_K); break;
				case 'L':			callInputListenersKeyUp(KEY_L); break;
				case 'Z':			callInputListenersKeyUp(KEY_Z); break;
				case 'X':			callInputListenersKeyUp(KEY_X); break;
				case 'C':			callInputListenersKeyUp(KEY_C); break;
				case 'V':			callInputListenersKeyUp(KEY_V); break;
				case 'B':			callInputListenersKeyUp(KEY_B); break;
				case 'N':			callInputListenersKeyUp(KEY_N); break;
				case 'M':			callInputListenersKeyUp(KEY_M); break;
				default:
					break;
				}
			}
#pragma endregion

		default:
			// We do not want to handle this message so pass back to Windows
			// to handle it in a default way
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}

		return 0;
	}

	bool ApplicationWindow_WGL::drawGLScene()
	{
		glClear(GL_COLOR_BUFFER_BIT);	// Since we're in 2D, only clear Screen Buffer and not the depth buffer
		glLoadIdentity();									// Reset The Current Modelview Matrix

		glColor3f(1,1,1);

		glPushMatrix();
		glTranslatef(-1.5f,0.0f,0.0f);						// Move Left 1.5 Units And Into The Screen 6.0
		glBegin(GL_TRIANGLES);								// Drawing Using Triangles
		glVertex2f( 0.0f, 1.0f);						// Top
		glVertex2f(-1.0f,-1.0f);						// Bottom Left
		glVertex2f( 1.0f,-1.0f);						// Bottom Right
		glEnd();											// Finished Drawing The Triangle
		glPopMatrix();

		glTranslatef(3.0f,0.0f,0.0f);						// Move Right 3 Units
		glBegin(GL_QUADS);									// Draw A Quad
		glVertex2f(-1.0f, 1.0f);						// Top Left
		glVertex2f( 1.0f, 1.0f);						// Top Right
		glVertex2f( 1.0f,-1.0f);						// Bottom Right
		glVertex2f(-1.0f,-1.0f);						// Bottom Left
		glEnd();											// Done Drawing The Quad


		return TRUE;										// Keep Going
	}



} // namespace PhySketch