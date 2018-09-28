#ifndef ENGINE_CVCOMMON_H
#define ENGINE_CVCOMMON_H

#include "YSEngineCommon.h"
#include "YSEngineBTCommon.h"

#if (defined WIN32 || defined _WIN32) && defined ENGINE_CV_EXPORTS
#	define YSENGINE_CV_API_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#	define YSENGINE_CV_API_EXPORTS __attribute__ ((visibility ("default")))
#else
#	define YSENGINE_CV_API_EXPORTS
#endif

#ifndef YSENGINE_CV_EXTERN_C
#	ifdef __cplusplus
#		define YSENGINE_CV_EXTERN_C extern "C"
#	else
#		define YSENGINE_CV_EXTERN_C
#	endif
#endif

#if defined WIN32 || defined _WIN32
#	define YSENGINE_CV_CDECL		__cdecl
#	define YSENGINE_CV_STDCALL	__stdcall
#else
#	define YSENGINE_CV_CDECL
#	define YSENGINE_CV_STDCALL
#endif

#ifndef YSENGINE_CV_API
#  define YSENGINE_CV_API(rettype) YSENGINE_CV_EXTERN_C YSENGINE_CV_API_EXPORTS rettype YSENGINE_CV_CDECL
#endif

#ifndef YSENGINE_CV_IMPL
#  define YSENGINE_CV_IMPL YSENGINE_CV_EXTERN_C
#endif

#ifndef YS_MIN
#define YS_MIN(a, b)             ( ((a) < (b)) ? (a) : (b) )
#endif

#ifndef YS_MAX
#define YS_MAX(a, b)             ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef YS_ABS
#define YS_ABS(a)                ( (a) < 0 ? (-(a)) : (a) )
#endif

#ifndef YS_SUB_ABS
#define YS_SUB_ABS(a, b)         ( (a) > (b) ? ((a) - (b)) : ((b) - (a)) )
#endif

#ifndef YS_SAFE_DELETE
#define YS_SAFE_DELETE(x)        { if(x) delete x; x = 0; }
#endif 

#ifndef YS_SAFE_DELETE_ARRAY
#define YS_SAFE_DELETE_ARRAY(x)  { if(x) delete[] x; x = 0; }
#endif

#define IMGSIZE_160X120

enum ResizeMethod
{
	NN_Method,
	AvgSquare,
	LinearWeight,
	NN_Bresenham
};

#ifndef int32_t
typedef int  int32_t;
#endif

#ifndef int64_t
#ifdef	WIN32
typedef __int64  int64_t;
#elif defined(__GNUC__)
typedef long long  int64_t;
#endif
#endif 

#endif	// ENGINE_CVCOMMON_H