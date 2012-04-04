namespace PhySketch
{
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

	// type defs
	typedef unsigned int uint;
	typedef unsigned long ulong;


	// Pre-declare classes so we can use pointers in header files 
	// without including individual .h
	class ApplicationWindow;
	class ApplicationWindow_WGL;
	class Core;
	class InputListener;
	class Logger;

} //namespace PhySketch


//////////////////////////////////////////////////////////////////////////
// include files
//
#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <list>
#include <set>

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

#endif //PHYSKETCH_OS_WIN32

