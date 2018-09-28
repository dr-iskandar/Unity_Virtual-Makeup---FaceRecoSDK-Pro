/* Copyright 2015 Shanghai Yingsui Technology Co., Ltd. */

#ifndef ENGINE_FFT_WRAPPER_H
#define ENGINE_FFT_WRAPPER_H

#include "YSEngineCommon.h"
#include "YSEngineFFTCommon.h"

#if (defined WIN32 || defined _WIN32) && defined YS_ENGINE_EXPORTS
#	define YS_ENGINE_FFT_API_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#	define YS_ENGINE_FFT_API_EXPORTS __attribute__ ((visibility ("default")))
#else
#	define YS_ENGINE_FFT_API_EXPORTS
#endif

#ifndef YS_ENGINE_FFT_EXTERN_C
#	ifdef __cplusplus
#		define YS_ENGINE_FFT_EXTERN_C extern "C"
#	else
#		define YS_ENGINE_FFT_EXTERN_C
#	endif
#endif

#if defined WIN32 || defined _WIN32
#	define YS_ENGINE_FFT_CDECL		__cdecl
#	define YS_ENGINE_FFT_STDCALL	__stdcall
#else
#	define YS_ENGINE_FFT_CDECL
#	define YS_ENGINE_FFT_STDCALL
#endif

#ifndef YS_ENGINE_FFT_API
#  define YS_ENGINE_FFT_API(rettype) YS_ENGINE_FFT_EXTERN_C YS_ENGINE_FFT_API_EXPORTS rettype YS_ENGINE_FFT_CDECL
#endif

#ifndef YS_ENGINE_FFT_IMPL
#  define YS_ENGINE_FFT_IMPL YS_ENGINE_FFT_EXTERN_C
#endif

/**
* Create FFT object, only call once
* @param[in]	license		license
*
* @return successful is YS_ENGINE_SUCCEED, other fail
*/
YS_ENGINE_FFT_API(int) YS_ENGINE_FFT_CreateFFT(const char* license);

/**
* Release FFT object, only call once
*/
YS_ENGINE_FFT_API(void) YS_ENGINE_FFT_ReleaseFFT();

/**
* Set video size, only support gray image
* @return successful is YS_ENGINE_SUCCEED, other fail
*/
YS_ENGINE_FFT_API(int) YS_ENGINE_FFT_Init(int width, int height, int nFormat = YS_ENGINE_VIDEO_GREY, int nRotation = 0, int nFlip = 0);

/**
* Set FFT Config
*/
YS_ENGINE_FFT_API(int) YS_ENGINE_FFT_SetConfig(const FFTConfig &config);

/**
* Get FFT Config
*/
YS_ENGINE_FFT_API(void) YS_ENGINE_FFT_GetConfig(FFTConfig &config);

/**
* do facial feature detection
* @param	bitmapPtr		: [i] input raw image data
* @param	bufferSize		: [i] width of input image
* @param	face			: [i] face rect
* @param	fFeaturePoints	: [o] allocated buffer to store detected 26 feature points position data
* @param	fFacePose		: [o] allocated buffer to store detected 6 face components: position x, y, z (scale), angle x, y, z
*
* @return	YS_ENGINE_SUCCEED if detected, other failed
*/
YS_ENGINE_FFT_API(int) YS_ENGINE_FFT_Detect(const void* bitmapPtr, int bufferSize, const YSFace& face, YSNode* fFeaturePoints, float* fFacePose);

/**
* tracker facial feature
* @param	bitmapPtr		: [i] input raw image data
* @param	bufferSize		: [i] width of input image
* @param	fFeaturePoints	: [o] allocated buffer to store detected 26 feature points position data
* @param	fFacePose		: [o] allocated buffer to store detected 6 face components: position x, y, z (scale), angle x, y, z
* @param	nWarning		: [o] warning information
*
* @return	YS_ENGINE_SUCCEED if tracked, other failed
*/
YS_ENGINE_FFT_API(int) YS_ENGINE_FFT_Track(const void* bitmapPtr, int bufferSize, YSNode* fFeaturePoints, float* fFacePose, int &iWarning);

/**
* @brief Track multiple faces
* @param	bitmapPtr		: [i] input raw image data
* @param	bufferSize		: [i] width of input image
* @param	maxNumFace		: [i] max number of faces
* @param	ppFaces			: [o] tracked faces
* @param	numFaces		: [o] number of tracked faces
* @return	YS_ENGINE_SUCCEED if tracked face, else other errors
*/
YS_ENGINE_FFT_API(int) YS_ENGINE_FFT_TrackMultiple(const void* bitmapPtr, int bufferSize, int maxNumFace, stFaceInfo **ppFaces, int &numFaces);

/**
* tracker facial feature
* @param	bitmapPtr		: [i] input gray raw image data
* @param	bufferSize		: [i] width of input image
* @param	leftX			: [o] left eye x pos
* @param	leftY			: [o] left eye y pos
* @param	rightX			: [o] right eye x pos
* @param	rightY			: [o] right eye y pos
* @param	fFacePose		: [o] allocated buffer to store detected 6 face components: position x, y, z (scale), angle x, y, z
*
* @return	true if Refine, false failed
*/
YS_ENGINE_FFT_API(bool) YS_ENGINE_FFT_RefineEyeByFFT(const void* bitmapPtr, int bufferSize, int& leftX, int& leftY, int& rightX, int& rightY, float *fFacePose);

/**
* global face moving distance
* @param	dx		: [i] face moving in x
* @param	dy		: [i] face moving in y
* @param	flag	: [i] moving part.0: all the 26 points; 1: left eye part; 2: right eye part; 3: mouth part
*/
YS_ENGINE_FFT_API(void) YS_ENGINE_FFT_GlobalMove(double dx, double dy, int flag = 0);

/**
* get expressions
* @param	fExpressions	: [o] expressions ratio, size = 18
* @return	true if successful, else false
*/
YS_ENGINE_FFT_API(bool) YS_ENGINE_FFT_GetExpressions(float *fExpressions);

/**
* get mapping projection matrix, need to call one time for each frame
*/
YS_ENGINE_FFT_API(void) YS_ENGINE_FFT_getMappingProjection();

/**
* mapping frontal 3D coordination to 2D tracking coordination
* @param	x	: [i] 3d coordinate X based on frontal model
* @param	y	: [i] 3d coordinate Y
* @param	z	: [i] 3d coordinate Z
* @param	xT	: [o] projected 2d coordinate X on video image
* @param	yT	: [o] projected 2d coordinate Y on video image
*/
YS_ENGINE_FFT_API(void) YS_ENGINE_FFT_mapping3D2Tracking(float x, float y, float z, float *xT, float *yT);

/**
* create a 3D model based on image plane point using one global tracking point
* @param	x	: [i] 2d coordinate X in image plane
* @param	y	: [i] 2d coordinate Y in image pane
* @param	xT	: [o] 3d model x
* @param	yT	: [o] 3d model y
* @param	zT	: [o] 3d model z
* @param	flag: [i] reference point: 0 = left eye point; 1 = right eye point; other = nose tip point
*/
YS_ENGINE_FFT_API(void) YS_ENGINE_FFT_CreateOutFace3DModel(float imgx, float imgy, float *xT, float *yT, float* zT, int flag);

/**
* feedback fitted user's 3d face model
* @param	p3dModel	: [o] 3d face model points
*/
YS_ENGINE_FFT_API(void) YS_ENGINE_FFT_Feedback3dFaceModel(YSNode *p3dModel);

/**
* enable remap 3d mask model to image
* @param	bMask3d		: [i] 3d mask remap flag
*/
YS_ENGINE_FFT_API(void) YS_ENGINE_FFT_EnableMaskRemapFrom3d(bool bMask3d);

/**
* estimate face skin contour
* @param	pBuff				: [i] input color buffer data used for facial tracking
* @param	szBuff				: [i] buffer size in bytes
* @param	pFaceSkinContour	: [o] face skin contour points, points number = 32
* @param	pAvgRGBEyeLeft		: [o] average rgb color of left eye, size should be 3 
* @param	pAvgRGBEyeRight		: [o] average rgb color of right eye, size should be 3 
* @return	YS_ENGINE_SUCCEED if found skin contour, other values if failed
*/
YS_ENGINE_FFT_API(int) YS_ENGINE_FFT_GetFaceSkinContour(const void *pBuff, int szBuff, YSNode *pFaceSkinContour, float *pAvgRGBEyeLeft = 0, float *pAvgRGBEyeRight = 0);

/**
* estimate face smooth factors map
* @param	pNodeFeaturePts		: [i] facial feature points
* @param	pNodeSkinContourPts	: [i] skin contour points
* @param	nWidth				: [i] width of video frame/image sync with feature points
* @param	nHeight				: [i] height of video frame/image sync with feature points
* @param	ptrSmoothingMap		: [o] output face smoothing factor map, must be allocated already by (nWidth/nScale) * (nHeight/nScale) (1 channel 8 bit gray image)
* @param	nScale				: [i] scale of map and video frame/image, i.e., nScale = frame size / map size
*/
YS_ENGINE_FFT_API(int) YS_ENGINE_FFT_GetFaceSmoothFactorMap(YSNode *pNodeFeaturePts, YSNode *pNodeSkinContourPts, int nWidth, int nHeight, void *ptrSmoothingMap, int nScale);

#endif//ENGINE_FFT_WRAPPER_H
