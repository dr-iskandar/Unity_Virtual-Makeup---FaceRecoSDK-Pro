#ifndef ENGINE_PLATFORM_H
#define ENGINE_PLATFORM_H

//---------------------------------------------------------------------------
// Platform Defines
//---------------------------------------------------------------------------
enum
{
	YS_ENGINE_PLATFORM_ANDROID_ARM   = 0x0001,
	YS_ENGINE_PLATFORM_LINUX_ARM     = 0x0002,
	YS_ENGINE_PLATFORM_LINUX_X86     = 0x0003,
	YS_ENGINE_PLATFORM_WINDOWS_NT    = 0x0004, // only 32 bit
};

//---------------------------------------------------------------------------
// Platform Identification 
//---------------------------------------------------------------------------
#ifdef ANDROID               // Android arm
	#define YS_ENGINE_PLAFORM           YS_ENGINE_PLATFORM_ANDROID_ARM
	#define YS_ENGINE_PLATFORM_STRING   "android-arm"
	#include "PlatformAndroid-Arm.h"
#elif defined(linux) && defined(__arm__)                 // Linux arm
	#define YS_ENGINE_PLAFORM           YS_ENGINE_PLATFORM_LINUX_ARM
	#define YS_ENGINE_PLATFORM_STRING   "linux-arm"
	#include "PlatformLinux-Arm.h"
#elif defined(linux) && defined(i386)                    // Linux x386
	#define YS_ENGINE_PLAFORM           YS_ENGINE_PLATFORM_LINUX_X86
	#define YS_ENGINE_PLATFORM_STRING   "linux-x86"
	#include "PlatformLinux-x86.h"
#elif defined(WIN32)                                    // Microsoft Visual Studio
	#define YS_ENGINE_PLAFORM           YS_ENGINE_PLATFORM_WINDOWS_NT
	#define YS_ENGINE_PLATFORM_STRING   "win32"
	#include "PlatformWin32.h"
#else 
	//#error YS_ENGINE Platform Abstraction Layer - Unsupported Platform!
    #define YS_ENGINE_PLATFORM           YS_ENGINE_PLATFORM_LINUX_ARM
    #define YS_ENGINE_PLATFORM_STRING    "osx"
    #include "PlatformLinux-Arm.h"
#endif

//---------------------------------------------------------------------------
// Base Macros
//---------------------------------------------------------------------------
#define YS_DISABLE_COPY(Class)		\
	Class (const Class&);				\
	Class &operator=(const Class&);

#define YS_ABS(a)                ( (a) < 0 ? (-(a)) : (a) )
#define YS_SUB_ABS(a, b)			( (a) > (b) ? ((a) - (b)) : ((b) - (a)) )
#define YS_MIN(a, b)             ( ((a) < (b)) ? (a) : (b) )
#define YS_MAX(a, b)             ( ((a) > (b)) ? (a) : (b) )
#define YS_SWAP(a, b, t)         ( (t) = (a); (a) = (b); (b) = (t); )

#define YS_SAFE_DELETE(x)        { if(x) delete x; x = 0; }
#define YS_SAFE_DELETE_ARRAY(x)  { if(x) delete[] x; x = 0; }

/** Converts n into a pre-processor string.  */ 
#define YS_STRINGIFY_HELPER(n)   #n
#define YS_STRINGIFY(n)          YS_STRINGIFY_HELPER(n)
#define YS_CONNECTION(a, b)      (a##b)

#define YS_GETLENGTH_ARRAY(a)         sizeof(a) / sizeof(a[0])
#define YS_STRCPY_ARRAY_SIZE(a, b, l) { if(a && b) strncpy(a, b, l); }
#define YS_STRCPY_ARRAY(a, b)         { if(a && b) { size_t len1 = sizeof(a); size_t len2 = sizeof(b); strncpy(a, b, len1 < len2 ? len1 : len2); }}

//---------------------------------------------------------------------------
// Digital Image Macros
//---------------------------------------------------------------------------
#define YS_PI                    3.141592653589793f
#define YS_RGB2GRAY(r, g, b)     ( ((b) * 117 + (g) * 601 + (r) * 306) >> 10 )
//#define YS_GetRValue(rgb)      ( (unsigned __int8)(rgb) )
//#define YS_GetGValue(rgb)      ( (unsigned __int8)(((signed __int16)(rgb)) >> 8) )
//#define YS_GetBValue(rgb)      ( (unsigned __int8)((rgb) >> 16) )
//#define YS_RGB(r,g,b)          ( (unsigned __int32)(((unsigned __int8)(r)|((signed __int16)((unsigned __int8)(g)) << 8))|(((uint32_t)(unsigned __int8)(b)) << 16)) )


/************************************************************************************* 
* Returns the date and time at compile time
**************************************************************************************/
#define YS_TIMESTAMP __DATE__ " " __TIME__

//---------------------------------------------------------------------------
// API Export/Import Macros
//---------------------------------------------------------------------------
#ifdef __cplusplus
	#define YS_ENGINE_C_API_EXPORT extern "C" YS_ENGINE_API_EXPORT
	#define YS_ENGINE_C_API_IMPORT extern "C" YS_ENGINE_API_IMPORT
	#define YS_ENGINE_CPP_API_EXPORT YS_ENGINE_API_EXPORT
	#define YS_ENGINE_CPP_API_IMPORT YS_ENGINE_API_IMPORT
#else
	#define YS_ENGINE_C_API_EXPORT   YS_ENGINE_API_EXPORT
	#define YS_ENGINE_C_API_IMPORT   YS_ENGINE_API_IMPORT
#endif

#ifdef YS_ENGINE_EXPORTS
	#define YS_ENGINE_C_API     YS_ENGINE_C_API_EXPORT
	#define YS_ENGINE_CPP_API   YS_ENGINE_CPP_API_EXPORT
#else
	#define YS_ENGINE_C_API     YS_ENGINE_C_API_IMPORT
	#define YS_ENGINE_CPP_API   YS_ENGINE_CPP_API_IMPORT
#endif

#endif	// ENGINE_PLATFORM_H
