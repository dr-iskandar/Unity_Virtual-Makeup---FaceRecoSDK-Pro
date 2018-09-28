#ifndef ENGINE_CV_EXT_H
#define ENGINE_CV_EXT_H

#include "YSEngineCVCommon.h"

//---------------------------------------------------------------------------
// mobiMemcpy.h
//---------------------------------------------------------------------------
/**
* @brief Copies count bytes from src to dest.
* @param	dest	pointer to destination buffer
* @param	src		pointer to source buffer
* @param	n		number of bytes to copy
*
* @return	A pointer to destination buffer
*/
YSENGINE_CV_API(void *) YSCV_Memcpy(void *dest , const void *src , unsigned int n);

/**
* @brief Convert YUYV 2 RGB24
* @param	yuv_buffer		input YUYV image
* @param	rgb_buffer		output RGB24 image
* @param	width			image size
* @param	height			image size
*/
YSENGINE_CV_API(void) YSCV_YUYV2RGB24(unsigned char *yuv_buffer, unsigned char *rgb_buffer, int width, int height);

/**
* convert RGB24 to NV21 (YUV420SP)
* @param	pRGB	: input rgb24 buffer pointer
* @param	pNV21	: output NV21 buffer pointer
* @param	width	: width of raw data
* @param	height	: height of raw data
*
* @return YS_ENGINE_SUCCEED if OK, other errors else
*/
YSENGINE_CV_API(int) YSCV_RGB2NV21(const void* pRGB, void* pNV21, int width, int height);

/**
* convert RGB24 to NV21 (YUV420SP)
* @param	pNV21	: input NV21 buffer pointer
* @param	pRGB	: output rgb24 buffer pointer
* @param	width	: width of raw data
* @param	height	: height of raw data
*
* @return YS_ENGINE_SUCCEED if OK, other errors else
*/
YSENGINE_CV_API(int) YSCV_NV21_2_RGB(const void* pNV21, void* pRGB, int width, int height, int nFlip = YS_ENGINE_MIRROR_NONE);

/**
* @brief Convert one pixel BGR value to gray
* @param	R	: [i] pixel R value, range [0, 255]
* @param	G	: [i] pixel G value, range [0, 255]
* @param	B	: [i] pixel B value, range [0, 255]
* @return	pixel gray value
*/
YSENGINE_CV_API(unsigned char) YSCV_BGR2GRAY_pixel(const unsigned char &R, const unsigned char &G, const unsigned char &B);

/**
* @brief Convert one pixel YCbCr(YUV) value to RGB
* @param	Y	: [i] pixel Y value, range [0, 255]
* @param	Cb	: [i] pixel Cb value, range [0, 255]
* @param	Cr	: [i] pixel Cr value, range [0, 255]
* @param	R	: [o] pixel R value, range [0, 255]
* @param	G	: [o] pixel G value, range [0, 255]
* @param	B	: [o] pixel B value, range [0, 255]
* @return	pixel gray value
*/
YSENGINE_CV_API(void) YSCV_YCbCr2RGB_pixel(const unsigned char &Y, const unsigned char &Cb, const unsigned char &Cr, unsigned char &R, unsigned char &G, unsigned char &B);

/**
* create The emergence of special effects, say: YU-HUA
* @param	cnt			: [I] number of control points
* @param	pts			: [I] control points
* @param	width		: [I] image width
* @param	height		: [I] image height
* @param	sz			: [I] effect size
* @param	pMask		: [O] output image, which get the effect by control points
*/
YSENGINE_CV_API(void) YSCV_SmoothFillPoints(int cnt, int * pts, int width, int height, int sz, unsigned char* pMask);


//---------------------------------------------------------------------------
// MobiColorFmtConversion.h
//---------------------------------------------------------------------------
/**
* @brief	Convert from NV21 640x480 to YUV 160x120
* @param	pNV21	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_NV21_640X480_2_YUV_160X120(const void *pNV21, void *pYUV);

/**
* @brief	Convert from NV21 720P to YUV 160x90
* @param	pNV21	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_NV21_1280X720_2_YUV_160X90(const void *pNV21, void *pYUV);

/**
* @brief	Convert from NV21 720P to YUV 320x180
* @param	pNV21	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_NV21_1280X720_2_YUV_320X180(const void *pNV21, void *pYUV);

//convert from NV21 1080P to YUV 160x90
/**
* @brief	Convert from NV21 1080P to YUV 160x90
* @param	pNV21	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_NV21_1920X1080_2_YUV_160X90(const void *pNV21, void *pYUV);

/**
* @brief	Convert from NV21 1080P to YUV 240x135
* @param	pNV21	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_NV21_1920X1080_2_YUV_240X135(const void *pNV21, void *pYUV);

/**
* @brief	Convert from NV12 640X480 to YUV 160x120
* @param	pNV12	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_NV12_640X480_2_YUV_160X120(const void *pNV12, void *pYUV);

/**
* @brief	Convert from NV12 720P to YUV 160x90
* @param	pNV12	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_NV12_1280X720_2_YUV_160X90(const void *pNV12, void *pYUV);

/**
* @brief	Convert from NV12 720P to YUV 320x180
* @param	pNV12	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_NV12_1280X720_2_YUV_320X180(const void *pNV12, void *pYUV);

/**
* @brief	Convert from YUYV422packed to YUV 160x120
* @param	pYUYV	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_YUYV422packed_640X480_2_YUV_160X120(const void *pYUYV, void *pYUV);

/**
* @brief	Convert from YUYV422packed 720P to YUV 160x90
* @param	pYUYV	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_YUYV422packed_1280X720_2_YUV_160X90(const void *pYUYV, void *pYUV);

/**
* @brief	Convert from YUYV422packed 720P to YUV 320x180
* @param	pYUYV	Input image 
* @param	pYUV	Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_YUYV422packed_1280X720_2_YUV_320X180(const void *pYUYV, void *pYUV);

//convert from RGB24 320X240 to BGR 160x120
/**
* @brief	Convert from RGB24 720P to BGR 160x90
* @param	pBGR24In	Input image 
* @param	pBGROut		Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_BGR24_320X240_2_BGR_160X120(const void *pBGR24In, void *pBGROut);

//convert from RGB24 640X480 to BGR 160x120
/**
* @brief	Convert from RGB24 720P to BGR 160x90
* @param	pBGR24In	Input image 
* @param	pBGROut		Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_BGR24_640X480_2_BGR_160X120(const void *pBGR24In, void *pBGROut);

/**
* @brief	Convert from RGB24 720P to BGR 160x90
* @param	pBGR24In	Input image 
* @param	pBGROut		Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_BGR24_1280X720_2_BGR_160X90(const void *pBGR24In, void *pBGROut);

YSENGINE_CV_API(bool) YSCV_BGR24_720X1280_2_BGR_90X160(const void *pBGR24In, void *pBGROut);

/**
* @brief	Convert from RGB24 720P to BGR 320x180
* @param	pBGR24In	Input image 
* @param	pBGROut		Output image
*
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_BGR24_1280X720_2_BGR_320X180(const void *pBGR24In, void *pBGROut);

/**
* @brief	Resize NV21 buffer with flip left-right: scale ratio is int
* @param	pSrc	: data buffer of source image
* @param	wSrc	: width of source image
* @param	hSrc	: height of source image
* @param	pDst	: buffer of dst image
* @param	wDst	: width of dst image
* @param	hDst	: height of dst image
* @return	
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_Resize_NV21_FlipY_Int(const void *pSrc, int wSrc, int hSrc, void *pDst, int wDst, int hDst);

/**
* @brief	Resize BGR buffer with flip left-right: scale ratio is int
* @param	pSrc	: data buffer of source image
* @param	wSrc	: width of source image
* @param	hSrc	: height of source image
* @param	pDst	: buffer of dst image
* @param	wDst	: width of dst image
* @param	hDst	: height of dst image
* @return
*	-<em>false</em> if argument invalid
*	-<em>true</em> otherwise return YS_ENGINE_SUCCEED
*/
YSENGINE_CV_API(bool) YSCV_Resize_BGR_FlipY_Int(const void *pSrc, int wSrc, int hSrc, void *pDst, int wDst, int hDst);

/**
* @brief	rotate and flip gray image raw buffer
* @param	pSrc	: input gray image data buffer
* @param	pDst	: output gray image data buffer by rotation and flip
* @param	width	: width of input gray image
* @param	height	: height of input gray image
* @param	nRotation	: rotation degree in anti-clockwise
* @param	nFlip	: flip direction
* @return	false if argument invalid, true else
*/
YSENGINE_CV_API(bool) YSCV_ROTATE_FLIP_GRAY_IMG(const void *pSrc, void *pDst, int width, int height, int nRotation, int nFlip);

/**
* @brief	abstract single channel from BGRA image with rotation and flip
* @param	pBGRA	: [i] BGRA image data buffer
* @param	pB		: [o] B channel data
* @param	pG		: [o] G channel data
* @param	pR		: [o] R channel data
* @param	width	: width of input gray image
* @param	height	: height of input gray image
* @param	nRotation	: rotation degree in anti-clockwise
* @param	nFlip	: flip direction
* @return	false if argument invalid, true else
*/
YSENGINE_CV_API(bool) YSCV_Split_BGRA_ROTATE_FLIP(const void *pBGRA, void *pB, void *pG, void *pR, int width, int height, int nRotation, int nFlip);

/**
* get brows off factor map
* @param	pNodeFeaturePts		: [i] facial feature points
* @param	numFeaturePts		: [i] number of facial feature points
* @param	pNodeSkinContourPts	: [i] facial skin contour points
* @param	numSkinPts			: [i] number of facial skin contour points
* @param	nWidth				: [i] width of video frame/image sync with feature points
* @param	nHeight				: [i] height of video frame/image sync with feature points
* @param	ptrMap				: [o] output factor map for brows off
* @param	fScale				: [o] scale value
* @param	xStart				: [o] top-left point x for cutting face rect
* @param	yStart				: [o] top-left point y for cutting face rect
* @param	nWidthMap			: [i] factor map width, same as scaled face width, default is 256
* @param	nHeightMap			: [i] factor map height, same as scaled face height, default is 256
* @return	YS_ENGINE_SUCCEED if OK, other value if error
*/
YSENGINE_CV_API(int) YSCV_GetBrowsOffFactorMap(YSNode *pNodeFeaturePts, int numFeaturePts, YSNode *pNodeSkinContourPts, int numSkinPts, int nWidth, int nHeight
											   , void *ptrMap, float &fScale, float &xStart, float &yStart, int nWidthMap = 256, int nHeightMap = 256);

/**
* get brows off factor map
* @param	pNodeFeaturePts		: [i] facial feature points
* @param	numFeaturePts		: [i] number of facial feature points
* @param	pNodeSkinContourPts	: [i] facial skin contour points
* @param	numSkinPts			: [i] number of facial skin contour points
* @param	nWidth				: [i] width of video frame/image sync with feature points
* @param	nHeight				: [i] height of video frame/image sync with feature points
* @param	ptrMap				: [o] output factor map for brows off
* @param	fScale				: [o] scale value
* @param	xStart				: [o] top-left point x for cutting face rect
* @param	yStart				: [o] top-left point y for cutting face rect
* @param	nWidthMap			: [i] factor map width, same as scaled face width, default is 256
* @param	nHeightMap			: [i] factor map height, same as scaled face height, default is 256
* @return	YS_ENGINE_SUCCEED if OK, other value if error
*/
YSENGINE_CV_API(int) YSCV_GetMouthOffFactorMap(YSNode *pNodeFeaturePts, int numFeaturePts, YSNode *pNodeSkinContourPts, int numSkinPts, int nWidth, int nHeight
											   , void *ptrMap, float &fScale, float &xStart, float &yStart, int nWidthMap = 256, int nHeightMap = 256);

//---------------------------------------------------------------------------
// mobiMath.h
//---------------------------------------------------------------------------
/**
* @brief	Fix div
* @param	a	
* @param	b	
* @param	p	
*
* @return	-1 or 1
*/
YSENGINE_CV_API(int) YSCV_Fixdiv(int32_t a, int32_t b, int p);

/**
* @brief	Fix atan
* @param	x	
*
* @return
*/
YSENGINE_CV_API(int32_t) YSCV_Fixatan_Alex(int32_t x);

/**
* @brief	atan integral
* @param	y	
* @param	x	
*
* @return
*/
YSENGINE_CV_API(int32_t) YSCV_Atan2_integral(int32_t y, int32_t x);

/**
* @brief	Fix sqrt
* @param	x	
* @return
*/
YSENGINE_CV_API(int32_t) YSCV_Fixsqrt16(int32_t x);


#endif