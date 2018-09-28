/* Copyright 2015 Shanghai Yingsui Technology Co., Ltd. */

#ifndef ENGINE_FD_WRAPPER_H
#define ENGINE_FD_WRAPPER_H

#include <vector>
#include "YSEngineCommon.h"
#include "YSEngineFFTCommon.h"

#if (defined WIN32 || defined _WIN32) && defined YS_ENGINE_EXPORTS
#	define YS_ENGINE_FD_API_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#	define YS_ENGINE_FD_API_EXPORTS __attribute__ ((visibility ("default")))
#else
#	define YS_ENGINE_FD_API_EXPORTS
#endif

#ifndef YS_ENGINE_FD_EXTERN_C
#	ifdef __cplusplus
#		define YS_ENGINE_FD_EXTERN_C extern "C"
#	else
#		define YS_ENGINE_FD_EXTERN_C
#	endif
#endif

#if defined WIN32 || defined _WIN32
#	define YS_ENGINE_FD_CDECL		__cdecl
#	define YS_ENGINE_FD_STDCALL	__stdcall
#else
#	define YS_ENGINE_FD_CDECL
#	define YS_ENGINE_FD_STDCALL
#endif

#ifndef YS_ENGINE_FD_API
#  define YS_ENGINE_FD_API(rettype) YS_ENGINE_FD_EXTERN_C YS_ENGINE_FD_API_EXPORTS rettype YS_ENGINE_FD_CDECL
#endif

#ifndef YS_ENGINE_FD_IMPL
#  define YS_ENGINE_FD_IMPL YS_ENGINE_FD_EXTERN_C
#endif


/**
* @brief Create FD object, only call once
* @param	license		: [i] license key
*
* @return YS_ENGINE_SUCCEED if succeed, other value if fail
*/
YS_ENGINE_FD_API(int) YS_ENGINE_FD_CreateFD(const char* license);

/**
* @brief Release FD object, only call once
*/
YS_ENGINE_FD_API(void) YS_ENGINE_FD_ReleaseFD();

/**
* @brief Init FD, this function need to be called first of all one time
* @param	width		: [i] width of processing image resolution
* @param	height		: [i] height of processing image resolution
*/
YS_ENGINE_FD_API(void) YS_ENGINE_FD_Init(int width, int height);

/**
* @brief Detect face in gray image
*
* @param	data		: [i] input gray image to detect face
* @param	roiRect		: [i] rect of interest
* @param	detectResult: [o] rect of detected face
* @param	sz			: [i] minimum face size to detect
*
* @return	true if face detected, false if detection failed
*/
YS_ENGINE_FD_API(bool) YS_ENGINE_FD_Detect(const void* data, YSRect& roiRect, YSRect& detectResult, int sz = 0);	

/**
* @brief Detect face and eye in gray image
*
* @param	data		: [i] input gray image to detect face
* @param	roiRect		: [i] rect of interest
* @param	face		: [o] rect of detected face
* @param	sz			: [i] minimum face size to detect
*
* @return	true if face detected, false if detection failed
*/
YS_ENGINE_FD_API(bool) YS_ENGINE_FD_DetectWithEye(const void* data, YSRect& roiRect, YSFace& face, int sz = 0);

/**
* @brief Detect face at give position
*
* @param	data		: [i] input gray image to detect face
* @param	roiRect		: [i] rect of interesting
* @param	xPos		: [i] x position
* @param	yPos		: [i] y position
* @param	face		: [o] detected face,
* @param	sz			: [i] minimum face size for detection
* 
* @return	true if face detected, false if detection failed
*/
YS_ENGINE_FD_API(bool) YS_ENGINE_FD_DetectFaceWithEyeAtPosition(const void* data, YSRect &roiRect, int xPos, int yPos, YSFace &face, int sz = 0);

/**
* @brief Detect face and eye in gray image
*
* @param	data			: [i] input gray image to detect face
* @param	imgMask			: [i] input mask image to filter face
* @param	roiRect			: [i] rect of interest
* @param	detectResult	: [o] rect of detected face
* @param	sz				: [i] minimum face size for detection
*
* @return	true if face detected, false if detection failed
*/
YS_ENGINE_FD_API(bool) YS_ENGINE_FD_Detect_mask(const void* data, const void*  imgMask, YSRect& roiRect, YSRect& detectResult, int sz = 0);

/**
* @brief Detect multiple faces with mask
*
* @param	data		: [i] input gray image to detect face
* @param	imgMask		: [i] input mask image to filter face
* @param	roiRect		: [i] rect of interest
* @param	results		: [o] rect of detected faces
* @param	sz			: [i] minimum face size for detection
*
* @return	true if face detected, false if detection failed
*/
YS_ENGINE_FD_API(bool) YS_ENGINE_FD_DetectMultiple_mask(const void* data, const void*  imgMask, YSRect& roiRect, std::vector<YSRect> &results, int sz = 0);

/**
* @brief Detect multiple faces by excluding existing faces
* 
* @param	data		: [i] input gray image to detect face
* @param	ppFaces		: [i/o] existed faces before detection, also detected face will be added as output
* @param	numFaces	: [i/o] existed face number, will be changed if new faces detected
* @param	rectROI		: [i] rect of interesting for face detection
* @param	numFaceMax	: [i] max number of faces for detection
* @param	szMinFace	: [i] minimum face size to detect
* 
* @return	true if face detected, false if failed
*/
YS_ENGINE_FD_API(bool) YS_ENGINE_FD_DetectMultipleExclusion(const void* data, stFaceInfo **ppFaces, int &numFaces
															, const YSRect &rectROI, int numFaceMax, int szMinFace = 0);


/************************************************************************
* Eye Detect
************************************************************************/

/**
* @brief Detect face and eye in gray image
*
* @param	data		: [i] input gray image to detect face
* @param	imgMask		: [i] input mask image to filter face
* @param	roiRect		: [i] rect of interest
* @param	face		: [o] rect of detected face
* @param	leftEye		: [o] rect of detected left eye
* @param	rightEye	: [o] rect of detected right eye
*
* @return	true if eye detected, false if detection failed
*/
YS_ENGINE_FD_API(bool) YS_ENGINE_FD_DetectEye(const void* data, YSRect& roiRect, YSFace& face, YSRect& leftEye, YSRect& rightEye);

/*
* @brief Face segmentation in original image, the face area has been give in faceRect
* @param	pInput			: [i] input RGB or ARGB image buffer, flag == 0: BGR; flag == 1 for ARGB
* @param	pOutput			: [o] output ARGB image in smaller face area
* @param	faceRect		: [i] the roi face rectangle, can be 1.5 or 2 times of detected face area
* @param	flag			: [i] indicate input buffer pixel format
* @param	segFaceWidth	: [o] segmentation face width
* @param	segFaceHeight	: [o] segmentation face height
* 
* @return	true if eye detected, false if detection failed
*/
YS_ENGINE_FD_API(bool) YS_ENGINE_FD_SegmentationFace( const void* pInput, void* pOutput, YSRect& faceRect, int flag, int &segFaceWidth, int &segFaceHeight);

#endif	//ENGINE_FD_WRAPPER_H
