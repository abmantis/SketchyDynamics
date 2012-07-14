// Define current OS
#if defined( __WIN32__ ) || defined( _WIN32 )
#   define PHYSKETCH_OS_WIN32
#else
#   define PHYSKETCH_OS_LINUX
#endif

/* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define PHYSKETCH_ARCH_64
#else
#   define PHYSKETCH_ARCH_32
#endif


#ifndef M_PI
	#define M_PI	3.14159265358979323846f  
#endif
#ifndef M_PI_2
	#define M_PI_2	1.57079632679489661923f
#endif
#ifndef M_PI_4
	#define M_PI_4	0.785398163397448309616f
#endif
//////////////////////////////////////////////////////////////////////////
// include files
//
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <sstream>
#include <unordered_map>
#include <GL/glew.h>

//////////////////////////////////////////////////////////////////////////
// Windows includes. 
#ifdef PHYSKETCH_OS_WIN32

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX						// Stop windows.h from messing up std::min

// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>

#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
//#include <GL/glu.h>

#endif //PHYSKETCH_OS_WIN32



namespace PhySketch
{
	//////////////////////////////////////////////////////////////////////////
	/// Custom ASSERT macro
	void __physketch_assert__(const char * file, unsigned int line, const char * expr, bool dump);

	#ifndef NDEBUG
	#define PHYSKETCH_ASSERT(expr) if (!(expr)) __physketch_assert__(__FILE__, __LINE__, #expr, true); 
	#else 
	#define PHYSKETCH_ASSERT(expr) if (!(expr)) __physketch_assert__(__FILE__, __LINE__, #expr, false); 
	#endif

	//////////////////////////////////////////////////////////////////////////
	/// Stringize macro
	//////////////////////////////////////////////////////////////////////////
	/// two macros ensures any macro passed will be expanded before being stringified
	#define STRINGIZE_DETAIL(x) #x
	#define STRINGIZE(x) STRINGIZE_DETAIL(x)

	// type defs
	typedef unsigned int uint;
	typedef unsigned long ulong;

	typedef std::map<std::string, std::string> ParameterMap;

	// Pre-declare classes so we can use pointers in header files 
	// without including individual .h
	class AABB;
	class AnimatedPolygon;
	class ApplicationWindow;
	class ApplicationWindow_WGL;
	class Core;
	class PhysicsBody;
	class PhysicsJoint;
	class PhysicsManager;
	class InputListener;
	class Logger;
	class MainInputListener;
	class Material;
	class MaterialManager;
	class Polygon;
	class Renderer;
	class Shader_GL;
	class ShaderProgram_GL;
	class SubPolygon;
	class Vector2;

} //namespace PhySketch



