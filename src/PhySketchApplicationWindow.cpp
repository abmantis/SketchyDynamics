
#include "PhySketchApplicationWindow.h"
#include "PhySketchLogger.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library

#define PHYSKETCH_WINDOW_CLASS_NAME "PhySketchAppWindow"
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
	wc.lpfnWndProc		= &ApplicationWindow_WGL::initialWndProc;					// WndProc Handles Messages
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
		NULL)))								// Dont Pass Anything To WM_CREATE
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

void ApplicationWindow_WGL::resizeGLScene(int width, int height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	// Using Ortho for 2D
	gluOrtho2D(-16, 16, -9, 9);
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
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, Msg, wParam, lParam);
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

} // namespace PhySketch