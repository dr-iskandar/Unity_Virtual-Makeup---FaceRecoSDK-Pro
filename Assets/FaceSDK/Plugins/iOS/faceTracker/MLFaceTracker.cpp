/**
* MLFaceTracker.cpp
*/


#include <math.h>
#include "YSFDWrapper.h"
#include "YSFFTWrapper.h"
#include "YSEngineCV_ext.h"
#include "MLFaceTracker.h"

//#include "ML_arm_neon_test.h"


#ifndef PI
#define PI	3.1415926535897932
#endif
#ifndef PI_HALF
#define PI_HALF		PI*0.5
#endif
#ifndef PI_DOUBLE
#define PI_DOUBLE	PI*2.0
#endif


//#define MY_LOG
#ifdef MY_LOG
#include <android/log.h>
#include <strings.h>
#define LOG_TAG "TRACKER"
#define LOG_INFO(...) ((void)__android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__))
#endif

static bool saveImageRawData(const char* filePath, const void* data, unsigned int size)
{
	FILE * imageFile = fopen(filePath, "a+");

	if(imageFile==NULL)
	{
#ifdef MY_LOG
		LOG_INFO("saveImageRawData [FAILED]");
#endif
		return false;
	}
	else
	{
		fwrite(data, 1, size, imageFile);
		fclose (imageFile);
#ifdef MY_LOG
		LOG_INFO("saveImageRawData [OK]");
#endif
	}

	return true;
}

bool NV21_2_YUV420p(const void* pNV21, void* pYUV420p, int width, int height)
{
	const unsigned char* pSrc = (unsigned char*)pNV21;
	unsigned char* pDst = (unsigned char*)pYUV420p;

	int nSize = width * height;

	//copy Y
	memcpy(pDst, pSrc, nSize * sizeof(unsigned char));

	//get Cb / Cr plane
	pSrc += nSize;
	unsigned char* pDstCb = pDst + nSize;
	unsigned char* pDstCr = pDst + nSize + nSize/4;

	for (int h = 0; h<height/2; ++h)
	{
		for (int w=0; w<width; w+=2)
		{
			*pDstCr++ = *pSrc++;
			*pDstCb++ = *pSrc++;
		}
	}

	return true;
}

MLFaceTracker::MLFaceTracker()
: m_pBufferRotate		(0)
, m_bAllocBufRot		(false)
, m_bInitEngine			(false)
, m_nWidth				(640)
, m_nHeight				(480)
, m_nWidthIn			(640)
, m_nHeightIn			(480)
, m_nSizeBufferIn		(640 * 480 * 3 / 2)
, m_nFormat				(YS_ENGINE_VIDEO_NV21)	// default video format as NV21
, m_nRotation			(0)						// default no rotation
, m_nMirror				(0)						// default no mirror
, m_bHasFace			(false)
, m_bHasFFT				(false)
, m_nSizeDetect			(480 / 5)
, m_bMask3d				(true)
, m_pMapFaceSmoothFactors(0)
, m_nScaleMap			(2)
, m_numMaxFace			(1)
, m_numFrameCheckFace	(8)
, m_nFrameNumFaceTracking(10)
, m_ppFacesMultiple		(0)
, m_numFaces			(0)
, m_numAllocatedFaces	(0)
{
	m_rectROI.yMin = 0;
	m_rectROI.yMax = m_nHeight - 1;
	m_rectROI.xMin = (m_nWidth - m_nHeight)>>1;
	m_rectROI.xMax = m_rectROI.xMin + m_nHeight - 1;

    m_nWidthstepIn = m_nWidthIn;
    m_nChannals = 1;
}

MLFaceTracker::~MLFaceTracker()
{
	if (m_bAllocBufRot && 0 != m_pBufferRotate)
	{
		free(m_pBufferRotate);
		m_pBufferRotate = 0;
	}
	if (m_pMapFaceSmoothFactors)
	{
		delete [] m_pMapFaceSmoothFactors;
		m_pMapFaceSmoothFactors = 0;
	}

	if (m_ppFacesMultiple != 0)
	{
		for (int i=0; i<m_numMaxFace; ++i)
		{
			if (m_ppFacesMultiple[i] != 0)
			{
				delete m_ppFacesMultiple[i];
				m_ppFacesMultiple[i] = 0;
			}
		}
		delete [] m_ppFacesMultiple;
		m_ppFacesMultiple = 0;
	}

	ReleaseEngine();
}

int MLFaceTracker::CreateEngine(const char *license)
{
	if (m_bInitEngine)
	{
		return YS_ENGINE_SUCCEED;
	}

	int ret = YS_ENGINE_FD_CreateFD(license);
	if (ret != YS_ENGINE_SUCCEED)
	{
		return ret;
	}

	ret = YS_ENGINE_FFT_CreateFFT(license);
	if (ret != YS_ENGINE_SUCCEED)
	{
		return ret;
	}

	YS_ENGINE_FD_Init(m_nWidth, m_nHeight);
	int	ret1 = YS_ENGINE_FFT_Init(m_nWidthIn, m_nHeightIn, m_nFormat, m_nRotation, m_nMirror);

	FFTConfig fftConfig;
	YS_ENGINE_FFT_GetConfig(fftConfig);

	//use face contour tracking
	fftConfig.modeFFT = FFT_68;
	YS_ENGINE_FFT_SetConfig(fftConfig);
	
    YS_ENGINE_FFT_EnableMaskRemapFrom3d(m_bMask3d); //default enable

	m_bInitEngine = true;
	
	return YS_ENGINE_SUCCEED;
}

void MLFaceTracker::ReleaseEngine()
{
	YS_ENGINE_FD_ReleaseFD();
	YS_ENGINE_FFT_ReleaseFFT();
}

int MLFaceTracker::SetVideoFormat(int nWidth, int nHeight, int nFormat, int nRotation, int nMirror)
{
	if (nWidth%4 != 0 || nHeight%4 != 0)
	{
		return YS_ENGINE_INVALID_ARG;	//4-align needed
	}
	if (m_nWidthIn != nWidth || m_nHeightIn != nHeight)
	{
		if (m_pMapFaceSmoothFactors)
		{
			delete [] m_pMapFaceSmoothFactors;
			m_pMapFaceSmoothFactors = 0;
		}
	}

	int nSizeOld = m_nWidth * m_nHeight;

	m_nFormat = nFormat;
	m_nRotation = nRotation;
	m_nWidthIn = nWidth;
	m_nHeightIn = nHeight;
	if (m_nRotation == 90 || m_nRotation == 270)
	{
		m_nWidth = nHeight;
		m_nHeight = nWidth;
	}
	else if (m_nRotation == 0 || m_nRotation == 180)
	{
		m_nWidth = nWidth;
		m_nHeight = nHeight;
	}
	else	//invalid
	{
		return YS_ENGINE_INVALID_ARG;
	}
	m_nMirror = nMirror;

	YS_ENGINE_FD_Init(m_nWidth, m_nHeight);

	int	ret1 = YS_ENGINE_FFT_Init(m_nWidthIn, m_nHeightIn, nFormat, m_nRotation, m_nMirror);

	//reset RGB buffer
	if (nWidth * nHeight > nSizeOld)
	{
        if (m_bAllocBufRot && m_pBufferRotate != 0)
        {
            free(m_pBufferRotate);
            m_pBufferRotate = 0;
        }
    }

	//roi RECT
	if (m_nHeight > m_nWidth)
	{
		m_rectROI.xMin = 0;
		m_rectROI.xMax = m_nWidth - 1;
		m_rectROI.yMin = (m_nHeight - m_nWidth)>>1;
		m_rectROI.yMax = m_nHeight - 1;
	}
	else
	{
		m_rectROI.yMin = 0;
		m_rectROI.yMax = m_nHeight - 1;
		m_rectROI.xMin = (m_nWidth - m_nHeight)>>1;
		m_rectROI.xMax = m_rectROI.xMin + m_nHeight - 1;
	}

    switch (m_nFormat)
    {
        case YS_ENGINE_VIDEO_NV21:
        case YS_ENGINE_VIDEO_NV12:
            m_nWidthstepIn = m_nWidthIn;
            m_nChannals = 1;
        	m_nSizeBufferIn = m_nWidthIn * m_nHeightIn * 3 / 2;
        	break;
        case YS_ENGINE_VIDEO_GREY:
            m_nWidthstepIn = m_nWidthIn;
            m_nChannals = 1;
        	m_nSizeBufferIn = m_nWidthIn * m_nHeightIn;
        	break;
        case YS_ENGINE_VIDEO_RGBA32:
            m_nWidthstepIn = m_nWidthIn * 4;
            m_nChannals = 4;
        	m_nSizeBufferIn = m_nWidthIn * m_nHeightIn * 4;
        	break;
		case YS_ENGINE_VIDEO_RGB24:
			m_nWidthstepIn = m_nWidthIn * 3;
			m_nChannals = 3;
			m_nSizeBufferIn = m_nWidthIn * m_nHeightIn * 3;
			break;
        default:
        	break;
    }

    m_nSizeDetect = YS_MIN(m_nWidth, m_nHeight) / 5;

	m_nWidthMap = m_nWidth / m_nScaleMap;
	m_nHeightMap = m_nHeight / m_nScaleMap;

	return ret1;
}

void MLFaceTracker::SetMinimumFaceSize(int nSize)
{
	m_nSizeDetect = nSize;
}

/**
* @brief Set max number of faces
* @param	numFace		: max number of faces to detect/track
*/
void MLFaceTracker::SetMaxNumFace(int numFace)
{
	if (m_numMaxFace == numFace)
	{
		return;
	}
	if (m_ppFacesMultiple == 0)
	{
		m_numMaxFace = numFace;
		return;
	}

	//keep current faces
	stFaceInfo **ppFace = new stFaceInfo*[numFace];
	memset(ppFace, 0, numFace * sizeof(stFaceInfo*));
	for (int i=0; i<YS_MIN(m_numMaxFace, numFace); ++i)
	{
		ppFace[i] = m_ppFacesMultiple[i];
	}

	//remove faces no needed
	for (int j=numFace; j<m_numMaxFace; ++j)
	{
		if (m_ppFacesMultiple[j])
		{
			delete m_ppFacesMultiple[j];
			m_ppFacesMultiple[j] = 0;
		}
	}

	delete [] m_ppFacesMultiple;
	m_ppFacesMultiple = ppFace;

	m_numMaxFace = numFace;
	if (m_numFaces > m_numMaxFace)
	{
		m_numFaces = m_numMaxFace;
	}
}

/**
* @brief Set interval for detection. 
* @param	numFrames	: how many frames detection one time
*/
void MLFaceTracker::SetDetectionInterval(int numFrames)
{
	m_numFrameCheckFace = numFrames;
}

/**
* @brief Detect/track multiple faces
* @param	ptrBuffer	: pointer to video frame raw data buffer
* @param	szBuffer	: size of buffer in bytes
* @param	ppFaces		: tracked faces
* @param	numFaces	: number of tracked faces
* @return	YS_ENGINE_SUCCEED if there's face, else other errors
*/
stFaceInfo** MLFaceTracker::ProcessFrameMultipleFace(void *ptrBuffer, int szBuffer, int *numFaces)
{
	int ret = YS_ENGINE_DETECT_FAIL;

	//init 
	if (0 == m_ppFacesMultiple)
	{
		m_ppFacesMultiple = new stFaceInfo*[m_numMaxFace];
		for (int i=0; i<m_numMaxFace; ++i)
		{
			m_ppFacesMultiple[i] = 0;
		}
		m_numFaces = 0;
	}

	//check if need to detect new face
	++m_nFrameNumFaceTracking;
	if (m_nFrameNumFaceTracking > m_numFrameCheckFace && m_numFaces < m_numMaxFace)
	{
		m_nFrameNumFaceTracking = 0;

		//detect new faces
		if (YS_ENGINE_SUCCEED == RotateFlipBuffer(ptrBuffer))	// abstract rotated gray channel image buffer for face detection
		{
			//rotate / flip face rect
			RotateFlipMultipleFaceRect();
			YS_ENGINE_FD_DetectMultipleExclusion(m_pBufferRotate, m_ppFacesMultiple, m_numFaces, m_rectROI, m_numMaxFace, m_nSizeDetect);
		}
	}

	//track existing face
	if (m_numFaces > 0)
	{
		ret = YS_ENGINE_FFT_TrackMultiple(ptrBuffer, szBuffer, m_numMaxFace, m_ppFacesMultiple, m_numFaces);
	}

	*numFaces = m_numFaces;
	return m_ppFacesMultiple;
}

void MLFaceTracker::UpdateFaceBoundingRectFromContour()
{
	int numPts = 66;		//NUM_CONTOUR_68
	float xMin = m_FeaturePoints[0].x;
	float yMin = m_FeaturePoints[0].y;
	float xMax = xMin;
	float yMax = yMin;
	for (int i=1; i<numPts; ++i)
	{
		float x = m_FeaturePoints[i].x;
		float y = m_FeaturePoints[i].y;
		if (xMin > x)		xMin = x;
		else if (xMax < x)	xMax = x;
		if (yMin > y)		yMin = y;
		else if (yMax < y)	yMax = y;
	}
	float w = xMax - xMin + 1;
	float h = yMax - yMin + 1;
	float szFaceHalf = YS_MAX(w, h) * 0.5f;

	//align object rect with pupils
	//left pupil
	float xPupilL = (m_FeaturePoints[36].x + m_FeaturePoints[37].x + m_FeaturePoints[38].x
			+ m_FeaturePoints[39].x + m_FeaturePoints[40].x + m_FeaturePoints[41].x) / 6.0f;
	float yPupilL = (m_FeaturePoints[36].y + m_FeaturePoints[37].y + m_FeaturePoints[38].y
			+ m_FeaturePoints[39].y + m_FeaturePoints[40].y + m_FeaturePoints[41].y) / 6.0f;
	float xPupilR = (m_FeaturePoints[42].x + m_FeaturePoints[43].x + m_FeaturePoints[44].x
				+ m_FeaturePoints[45].x + m_FeaturePoints[46].x + m_FeaturePoints[47].x) / 6.0f;
	float yPupilR = (m_FeaturePoints[42].y + m_FeaturePoints[43].y + m_FeaturePoints[44].y
				+ m_FeaturePoints[45].y + m_FeaturePoints[46].y + m_FeaturePoints[47].y) / 6.0f;
	float dxPupil = xPupilL - xPupilR;
	float dyPupil = yPupilL - yPupilR;
	float dPupil = sqrtf(dxPupil * dxPupil + dyPupil * dyPupil);
	float xCenter = (xPupilL + xPupilR) * 0.5f;
	float yCenter = (yPupilL + yPupilR) * 0.5f;	// + 0.75f * dPupil

	m_rectTracking.xMin = xMin;
	m_rectTracking.xMax = xMax;
	m_rectTracking.yMin = yMin;
	m_rectTracking.yMax = yMax;
}

void MLFaceTracker::RotateFlip2OriginalView( YSNode *pNodeRotated, YSNode *pNodeOriginal, int numPts)
{
	//convert to original view
	if (m_nRotation == 90)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			for (int i=0; i < numPts; ++i)
			{
				pNodeOriginal[i].x = m_nWidthIn - 1 - pNodeRotated[i].y;
				pNodeOriginal[i].y = m_nHeightIn - 1 - pNodeRotated[i].x;
			}
		}
		else if (m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			for (int i=0; i < numPts; ++i)
			{
				pNodeOriginal[i].x = m_nWidthIn - 1 - pNodeRotated[i].y;
				pNodeOriginal[i].y = pNodeRotated[i].x;
			}
		}
	}
	else if (m_nRotation == 270)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			for (int i=0; i < numPts; ++i)
			{
				pNodeOriginal[i].x = pNodeRotated[i].y;
				pNodeOriginal[i].y = pNodeRotated[i].x;
			}
		}
		else if (m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			for (int i=0; i < numPts; ++i)
			{
				pNodeOriginal[i].x = pNodeRotated[i].y;
				pNodeOriginal[i].y = m_nHeightIn - 1 - pNodeRotated[i].x;
			}
		}
	}
	else if (m_nRotation == 0)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			for (int i=0; i < numPts; ++i)
			{
				pNodeOriginal[i].x = m_nWidthIn - 1 - pNodeRotated[i].x;
				pNodeOriginal[i].y = pNodeRotated[i].y;
			}
		}
		else if (m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			for (int i=0; i < numPts; ++i)
			{
				pNodeOriginal[i].x = pNodeRotated[i].x;
				pNodeOriginal[i].y = pNodeRotated[i].y;
			}
		}
	}
	else if (m_nRotation == 180)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			for (int i=0; i < numPts; ++i)
			{
				pNodeOriginal[i].x = pNodeRotated[i].x;
				pNodeOriginal[i].y = m_nHeightIn - 1 - pNodeRotated[i].y;
			}
		}
		else if (m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			for (int i=0; i < numPts; ++i)
			{
				pNodeOriginal[i].x = m_nWidthIn - 1 - pNodeRotated[i].x;
				pNodeOriginal[i].y = m_nHeightIn - 1 - pNodeRotated[i].y;
			}
		}
	}
}

int MLFaceTracker::DetectFace(void *ptrBuffer, int sizeBuffer, int xPos, int yPos, YSRect rectROI, int nSizeDetectMin)
{
	//processing tracking
	if (m_nFormat != YS_ENGINE_VIDEO_NV21 && m_nFormat != YS_ENGINE_VIDEO_GREY
			&& m_nFormat != YS_ENGINE_VIDEO_NV12 && m_nFormat != YS_ENGINE_VIDEO_RGBA32)
	{
		return YS_ENGINE_NOT_IMPLEMENTED;
	}

	int ret = YS_ENGINE_SUCCEED;

	if (!m_bHasFace)
	{
		//rotation
		switch (m_nFormat)
		{
			case YS_ENGINE_VIDEO_NV21:
			case YS_ENGINE_VIDEO_GREY:
			case YS_ENGINE_VIDEO_NV12:
			{
				if (m_nRotation == 0 && m_nMirror == 0)
				{
					m_pBufferRotate = (unsigned char*)ptrBuffer;
				}
				else
				{
					int nSize = m_nWidth * m_nHeight;
					if (m_pBufferRotate == 0)
					{
						m_pBufferRotate = (unsigned char *)malloc(nSize);
						m_bAllocBufRot = true;
					}
					bool bRet = YSCV_ROTATE_FLIP_GRAY_IMG(ptrBuffer, m_pBufferRotate, m_nWidthIn, m_nHeightIn, m_nRotation, m_nMirror);

					if (!bRet)
					{
						return YS_ENGINE_INVALID_ARG;
					}
				}
			}
			break;

			case YS_ENGINE_VIDEO_RGBA32:
			{
				int nSize = m_nWidth * m_nHeight;
				if (m_pBufferRotate == 0)
				{
					m_pBufferRotate = (unsigned char *)malloc(nSize);
					m_bAllocBufRot = true;
				}
				bool bRet = YSCV_Split_BGRA_ROTATE_FLIP(ptrBuffer, 0, 0, m_pBufferRotate, m_nWidthIn, m_nHeightIn, m_nRotation, m_nMirror);
				if (!bRet)
				{
					return YS_ENGINE_INVALID_ARG;
				}
			}
			break;

			default:
				return YS_ENGINE_NOT_IMPLEMENTED;
		}
	}
	unsigned char* pBufIn = m_pBufferRotate;

	//face detection for initialization
	bool retDetect = false;
	if (!m_bHasFace)
	{
		//detect multiple faces
		std::vector<YSRect> faceRects;
		retDetect = YS_ENGINE_FD_DetectMultiple_mask(pBufIn, 0, rectROI, faceRects, nSizeDetectMin);

		if (retDetect)
		{
			//get closest face to position
			for (int i=0; i<faceRects.size(); ++i)
			{
				int wFace = faceRects[i].xMax - faceRects[i].xMin + 1;
				int hFace = faceRects[i].yMax - faceRects[i].yMin + 1;
				int xOffset = wFace / 4;
				int yOffset = hFace * 0.45f;
				int yOffsetUp = hFace * 0.55f;
				if (xPos >= faceRects[i].xMin - xOffset && xPos <= faceRects[i].xMax + xOffset
						&& yPos >= faceRects[i].yMin - yOffsetUp && yPos <= faceRects[i].yMax + yOffset)
				{
					m_bHasFace = true;
					m_Face.rectFace = faceRects[i];
					break;
				}
			}

			//detect eyes
			if (m_bHasFace)
			{
				YSRect rectEyeLeft;
				YSRect rectEyeRight;
				retDetect = YS_ENGINE_FD_DetectEye(pBufIn, m_Face.rectFace, m_Face, rectEyeLeft, rectEyeRight);
				if (!retDetect)
				{
					m_bHasFace = false;
				}
			}
			else
			{
				retDetect = false;
			}
		}
		else
		{}

		//
		if (true == retDetect)
		{
			m_FeaturePoints[1].x = m_Face.nodeLP.x;
			m_FeaturePoints[1].y = m_Face.nodeLP.y;
			m_FeaturePoints[0].x = m_Face.nodeRP.x;
			m_FeaturePoints[0].y = m_Face.nodeRP.y;
			m_bHasFace = true;
		}
	}
	if (!m_bHasFace)
	{
		return YS_ENGINE_DETECT_FAIL;
	}

	int sz4 = m_nWidth * m_nHeight * 4;
	if (!m_bHasFFT)
	{
		if (m_nFormat == YS_ENGINE_VIDEO_RGBA32)
			ret = YS_ENGINE_FFT_Detect(ptrBuffer, sz4, m_Face, m_FeaturePoints, m_fFacePose);
		else
			ret = YS_ENGINE_FFT_Detect(ptrBuffer, sizeBuffer, m_Face, m_FeaturePoints, m_fFacePose);

		if (YS_ENGINE_SUCCEED == ret)
		{
			m_bHasFFT = true;
		}
		else
		{
			m_bHasFace = false;
			return YS_ENGINE_DETECT_FAIL;
		}
	}

	return ret;
}

int MLFaceTracker::ProcessFrame(void *ptrBuffer, int sizeBuffer)
{
#if 0
//test
static int nSaveNum = 0;
++nSaveNum;
if (nSaveNum < 100)
{
	saveImageRawData("/sdcard/Download/inputVideo.raw", ptrBuffer, sizeBuffer);
}
return -1;
#endif

	++m_nFrameNumFaceTracking;

	//process tracking
	if (m_nFormat != YS_ENGINE_VIDEO_NV21 && m_nFormat != YS_ENGINE_VIDEO_GREY && m_nFormat != YS_ENGINE_VIDEO_NV12
        && m_nFormat != YS_ENGINE_VIDEO_RGBA32)
	{
		return YS_ENGINE_NOT_IMPLEMENTED;
	}

	if (sizeBuffer < m_nSizeBufferIn)
	{
		return YS_ENGINE_INVALID_ARG;
	}

	//face detection for initialization
	bool retDetect = false;
	if(!m_bHasFace)
	{
		if (m_nFrameNumFaceTracking > m_numFrameCheckFace)		//detect each 5 frames
		{
			m_nFrameNumFaceTracking = 0;
			int ret = RotateFlipBuffer(ptrBuffer);
			if (YS_ENGINE_SUCCEED != ret)
			{
				return ret;
			}

            retDetect = YS_ENGINE_FD_DetectWithEye(m_pBufferRotate, m_rectROI, m_Face, m_nSizeDetect);
			if ( retDetect )
			{
				m_FeaturePoints[1].x = m_Face.nodeLP.x;
				m_FeaturePoints[1].y = m_Face.nodeLP.y;
				m_FeaturePoints[0].x = m_Face.nodeRP.x;
				m_FeaturePoints[0].y = m_Face.nodeRP.y;

				m_bHasFace = true;
			}
		}
	}
	if (!m_bHasFace)
	{
		return YS_ENGINE_DETECT_FAIL;
	}

	int sz4 = m_nWidth * m_nHeight * 4;
	if(!m_bHasFFT)
	{
		int ret;
		if (m_nFormat == YS_ENGINE_VIDEO_RGBA32)
			ret = YS_ENGINE_FFT_Detect(ptrBuffer, sz4, m_Face, m_FeaturePoints, m_fFacePose);
		else
			ret = YS_ENGINE_FFT_Detect(ptrBuffer, sizeBuffer, m_Face, m_FeaturePoints, m_fFacePose);

		if(YS_ENGINE_SUCCEED == ret)
		{
			m_bHasFFT = true;
		}
		else
		{
			m_bHasFace = false;
			return YS_ENGINE_DETECT_FAIL;
		}
	}
	else
	{
		int iWarning = 0;

		int ret;
		if (m_nFormat == YS_ENGINE_VIDEO_RGBA32)
			ret = YS_ENGINE_FFT_Track(ptrBuffer, sz4, m_FeaturePoints, m_fFacePose, iWarning);
		else
			ret = YS_ENGINE_FFT_Track(ptrBuffer, sizeBuffer, m_FeaturePoints, m_fFacePose, iWarning);

		if(YS_ENGINE_SUCCEED != ret)
		{
			m_bHasFace = false;
			m_bHasFFT = false;
			return YS_ENGINE_TRACK_FAIL;
		}
	}

	//get expressions
	bool bExp = YS_ENGINE_FFT_GetExpressions(m_fExpressions);

	return YS_ENGINE_SUCCEED;
}

int MLFaceTracker::UpdateTrackingData(YSNode *pFeaturePoints, float *pFacePose, float *pExpressions)
{
	if (!m_bHasFFT)
	{
		return YS_ENGINE_TRACK_FAIL;
	}

	int numPts = NUM_FFT_68;
	if (m_bMask3d)
    {
        numPts = NUM_MASK_MODEL_78;
    }

	//memcpy(pFeaturePoints, m_FeaturePoints, numPts * sizeof(YSNode));
	RotateFlip2OriginalView(m_FeaturePoints, pFeaturePoints, numPts);
	memcpy(pExpressions, m_fExpressions, EM_NUM_TOTAL * sizeof(float) );

	memcpy(pFacePose, m_fFacePose, NUM_POSE_6 * sizeof(float));
	//FacePose_RotateFlip2OriginalView(pFacePose);

	return YS_ENGINE_SUCCEED;
}

int MLFaceTracker::UpdateTrackingData(YSNode *pFeaturePoints, float *pFacePose, float *pExpressions, YSNode *p3dUserFace, YSRect *rect)
{
	if (!m_bHasFFT)
	{
		return YS_ENGINE_TRACK_FAIL;
	}

    int numPts = NUM_FFT_68;
    if (m_bMask3d)
    {
        numPts = NUM_MASK_MODEL_78;
    }

	//memcpy(pFeaturePoints, m_FeaturePoints, numPts * sizeof(YSNode));
	RotateFlip2OriginalView(m_FeaturePoints, pFeaturePoints, numPts);
	memcpy(pExpressions, m_fExpressions, EM_NUM_TOTAL * sizeof(float));

	memcpy(pFacePose, m_fFacePose, NUM_POSE_6 * sizeof(float));
	//FacePose_RotateFlip2OriginalView(pFacePose);
	YS_ENGINE_FFT_Feedback3dFaceModel(p3dUserFace);

	return YS_ENGINE_SUCCEED;
}

/**
* transform face pose and user 3d model to original view
* @param	pfPoseOriginal			: face pose in original view
*/
void MLFaceTracker::FacePose_RotateFlip2OriginalView( float *pfPoseOriginal)
{
	//convert to original view
	if (m_nRotation == 90)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			//face pose
			pfPoseOriginal[0] = - m_fFacePose[1];	//dx
			pfPoseOriginal[1] = - m_fFacePose[0];	//dy
			pfPoseOriginal[2] = m_fFacePose[2];		//dz
			pfPoseOriginal[3] = m_fFacePose[4];		//ax
			pfPoseOriginal[4] = m_fFacePose[3];		//ay
			pfPoseOriginal[5] = - m_fFacePose[5] - PI_HALF;	//az
		}
		else if (m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			//face pose
			pfPoseOriginal[0] = - m_fFacePose[1];	//dx
			pfPoseOriginal[1] = m_fFacePose[0];		//dy
			pfPoseOriginal[2] = m_fFacePose[2];		//dz
			pfPoseOriginal[3] = - m_fFacePose[4];	//ax
			pfPoseOriginal[4] = m_fFacePose[3];		//ay
			pfPoseOriginal[5] = m_fFacePose[5] - PI_HALF;		//az
		}
	}
	else if (m_nRotation == 270)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			//face pose
			pfPoseOriginal[0] = m_fFacePose[1];		//dx
			pfPoseOriginal[1] = m_fFacePose[0];		//dy
			pfPoseOriginal[2] = m_fFacePose[2];		//dz
			pfPoseOriginal[3] = m_fFacePose[4];		//ax
			pfPoseOriginal[4] = - m_fFacePose[3];	//ay
			pfPoseOriginal[5] = - m_fFacePose[5] + PI_HALF;	//az
		}
		else if (m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			//face pose
			pfPoseOriginal[0] = m_fFacePose[1];		//dx
			pfPoseOriginal[1] = - m_fFacePose[0];	//dy
			pfPoseOriginal[2] = m_fFacePose[2];		//dz
			pfPoseOriginal[3] = m_fFacePose[4];		//ax
			pfPoseOriginal[4] = - m_fFacePose[3];	//ay
			pfPoseOriginal[5] = m_fFacePose[5] + PI_HALF;	//az
		}
	}
	else if (m_nRotation == 0)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			//face pose
			pfPoseOriginal[0] = - m_fFacePose[0];	//dx
			pfPoseOriginal[1] = m_fFacePose[1];		//dy
			pfPoseOriginal[2] = m_fFacePose[2];		//dz
			pfPoseOriginal[3] = m_fFacePose[3];		//ax
			pfPoseOriginal[4] = - m_fFacePose[4];	//ay
			pfPoseOriginal[5] = - m_fFacePose[5];	//az
		}
		else if (m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			//face pose
			pfPoseOriginal[0] = m_fFacePose[0];		//dx
			pfPoseOriginal[1] = m_fFacePose[1];		//dy
			pfPoseOriginal[2] = m_fFacePose[2];		//dz
			pfPoseOriginal[3] = m_fFacePose[3];		//ax
			pfPoseOriginal[4] = m_fFacePose[4];		//ay
			pfPoseOriginal[5] = m_fFacePose[5];		//az
		}
	}
	else if (m_nRotation == 180)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			//face pose
			pfPoseOriginal[0] = m_fFacePose[0];		//dx
			pfPoseOriginal[1] = - m_fFacePose[1];	//dy
			pfPoseOriginal[2] = m_fFacePose[2];		//dz
			pfPoseOriginal[3] = - m_fFacePose[3];	//ax
			pfPoseOriginal[4] = m_fFacePose[4];		//ay
			pfPoseOriginal[5] = - m_fFacePose[5] - PI;	//az
		}
		else if (m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			//face pose
			pfPoseOriginal[0] = - m_fFacePose[0];	//dx
			pfPoseOriginal[1] = - m_fFacePose[1];	//dy
			pfPoseOriginal[2] = m_fFacePose[2];		//dz
			pfPoseOriginal[3] = - m_fFacePose[3];	//ax
			pfPoseOriginal[4] = - m_fFacePose[4];	//ay
			pfPoseOriginal[5] = m_fFacePose[5] - PI;	//az
		}
	}
}

int MLFaceTracker::GetFaceSkinContour(void *ptrBuffer, int sizeBuffer)
{
	if (!m_bHasFFT)
	{
		return YS_ENGINE_DETECT_FAIL;
	}

    return YS_ENGINE_FFT_GetFaceSkinContour(ptrBuffer, sizeBuffer, m_pFaceSkinContour, m_pAvgRGBLeftEye, m_pAvgRGBRightEye);
}

/**
 * get user 3d face model
 */
void MLFaceTracker::Feedback3dUserFaceModel()
{
	YS_ENGINE_FFT_Feedback3dFaceModel(m_pUser3dFaceModel);

	//LOG_INFO("[Engine-3dmodel]nFrame = %d, mode=%d, x0=%f, y0=%f", m_nFrameNumFaceTracking, m_nTrackingMode, m_pUser3dFaceModel[0].x, m_pUser3dFaceModel[0].y);
}

void MLFaceTracker::Mapping3D2Tracking(float x, float y, float z, float *xT, float *yT)
{
	YS_ENGINE_FFT_mapping3D2Tracking(x, y, z, xT, yT);
}

/**
* get face smoothing factors map for face beautify
*/
int MLFaceTracker::GetFaceSmoothFactorMap(int nScaleMap)
{
	if (nScaleMap <= 0)
	{
		return YS_ENGINE_INVALID_ARG;	//invalid scale
	}

	if (nScaleMap != m_nScaleMap)
	{
		if (m_pMapFaceSmoothFactors)
		{
			delete [] m_pMapFaceSmoothFactors;
			m_pMapFaceSmoothFactors = 0;
		}
	}
	if (0 == m_pMapFaceSmoothFactors)
	{
		m_nScaleMap = nScaleMap;
		m_nWidthMap = m_nWidth / m_nScaleMap;
		m_nHeightMap = m_nHeight / m_nScaleMap;
		m_pMapFaceSmoothFactors = new unsigned char[m_nWidthMap * m_nHeightMap];
	}

	return YS_ENGINE_FFT_GetFaceSmoothFactorMap(m_FeaturePoints, m_pFaceSkinContour, m_nWidthMap, m_nHeightMap, m_pMapFaceSmoothFactors, m_nScaleMap);
}

int MLFaceTracker::RotateFlipBuffer(const void *ptrBuffer)
{
	//rotation
	switch (m_nFormat)
	{
	case YS_ENGINE_VIDEO_NV21:
	case YS_ENGINE_VIDEO_GREY:
	case YS_ENGINE_VIDEO_NV12:
		{
			if (m_nRotation == 0 && m_nMirror == 0)
			{
				if (m_bAllocBufRot && 0 != m_pBufferRotate)
				{
					free(m_pBufferRotate);
				}
				m_bAllocBufRot = false;
				m_pBufferRotate = (unsigned char*)ptrBuffer;
			}
			else
			{
				int nSize = m_nWidth * m_nHeight;
				if (!m_bAllocBufRot || m_pBufferRotate == 0)
				{
					m_pBufferRotate = (unsigned char *)malloc(nSize);
					m_bAllocBufRot = true;
				}
				bool bRet = YSCV_ROTATE_FLIP_GRAY_IMG(ptrBuffer, m_pBufferRotate, m_nWidthIn, m_nHeightIn, m_nRotation, m_nMirror);

				if (!bRet)
				{
					return YS_ENGINE_INVALID_ARG;
				}
			}
		}
		break;

	case YS_ENGINE_VIDEO_RGBA32:
		{
			int nSize = m_nWidth * m_nHeight;
			if (!m_bAllocBufRot || m_pBufferRotate == 0)
			{
				m_pBufferRotate = (unsigned char *)malloc(nSize);
				m_bAllocBufRot = true;
			}
			bool bRet = YSCV_Split_BGRA_ROTATE_FLIP(ptrBuffer, 0, 0, m_pBufferRotate, m_nWidthIn, m_nHeightIn, m_nRotation, m_nMirror);
			if (!bRet)
			{
				return YS_ENGINE_INVALID_ARG;
			}
		}
		break;

	default:
		return YS_ENGINE_NOT_IMPLEMENTED;
		break;
	}

	return YS_ENGINE_SUCCEED;
}

//remap face rect to rotation view
void MLFaceTracker::RotateFlipMultipleFaceRect()
{
	if (m_numFaces <= 0)
		return;

	if(m_nRotation == YS_ENGINE_ROTATION_CLK_270)
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			for (int n=0; n<m_numFaces; ++n)
			{
				int yMin = m_ppFacesMultiple[n]->rectFace.xMin;
				int yMax = m_ppFacesMultiple[n]->rectFace.xMax;
				m_ppFacesMultiple[n]->rectFace.xMin = m_ppFacesMultiple[n]->rectFace.yMin;
				m_ppFacesMultiple[n]->rectFace.xMax = m_ppFacesMultiple[n]->rectFace.yMax;
				m_ppFacesMultiple[n]->rectFace.yMin = yMin;
				m_ppFacesMultiple[n]->rectFace.yMax = yMax;
			}
		}
		else if(m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			int h_1 = m_nHeightIn - 1;
			for (int n=0; n<m_numFaces; ++n)
			{
				int yMin = m_ppFacesMultiple[n]->rectFace.xMin;
				int yMax = m_ppFacesMultiple[n]->rectFace.xMax;
				m_ppFacesMultiple[n]->rectFace.xMin = h_1 - m_ppFacesMultiple[n]->rectFace.yMax;
				m_ppFacesMultiple[n]->rectFace.xMax = h_1 - m_ppFacesMultiple[n]->rectFace.yMin;
				m_ppFacesMultiple[n]->rectFace.yMin = yMin;
				m_ppFacesMultiple[n]->rectFace.yMax = yMax;
			}
		}
	}
	else if(m_nRotation == YS_ENGINE_ROTATION_CLK_90)
	{
		int w_1 = m_nWidthIn - 1;
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			int h_1 = m_nHeightIn - 1;
			for (int n=0; n<m_numFaces; ++n)
			{
				int yMin = w_1 - m_ppFacesMultiple[n]->rectFace.xMax;
				int yMax = w_1 - m_ppFacesMultiple[n]->rectFace.xMin;
				m_ppFacesMultiple[n]->rectFace.xMin = h_1 - m_ppFacesMultiple[n]->rectFace.yMax;
				m_ppFacesMultiple[n]->rectFace.xMax = h_1 - m_ppFacesMultiple[n]->rectFace.yMin;
				m_ppFacesMultiple[n]->rectFace.yMin = yMin;
				m_ppFacesMultiple[n]->rectFace.yMax = yMax;
			}
		}
		else if(m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			for (int n=0; n<m_numFaces; ++n)
			{
				int yMin = w_1 - m_ppFacesMultiple[n]->rectFace.xMax;
				int yMax = w_1 - m_ppFacesMultiple[n]->rectFace.xMin;
				m_ppFacesMultiple[n]->rectFace.xMin = m_ppFacesMultiple[n]->rectFace.yMin;
				m_ppFacesMultiple[n]->rectFace.xMax = m_ppFacesMultiple[n]->rectFace.yMax;
				m_ppFacesMultiple[n]->rectFace.yMin = yMin;
				m_ppFacesMultiple[n]->rectFace.yMax = yMax;
			}
		}
	}
	else if(m_nRotation == YS_ENGINE_ROTATION_NONE)	//0
	{
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			int w_1 = m_nWidthIn - 1;
			for (int n=0; n<m_numFaces; ++n)
			{
				int xMax = w_1 - m_ppFacesMultiple[n]->rectFace.xMin;
				m_ppFacesMultiple[n]->rectFace.xMin = w_1 - m_ppFacesMultiple[n]->rectFace.xMax;
				m_ppFacesMultiple[n]->rectFace.xMax = xMax;
			}
		}
		//else if(m_nMirror == YS_ENGINE_MIRROR_NONE)	//same coordinate
	}
	else if(m_nRotation == YS_ENGINE_ROTATION_CLK_180)
	{
		int h_1 = m_nHeightIn - 1;
		if (m_nMirror == YS_ENGINE_MIRROR_Y)
		{
			for (int n=0; n<m_numFaces; ++n)
			{
				int yMax = h_1 - m_ppFacesMultiple[n]->rectFace.yMin;
				m_ppFacesMultiple[n]->rectFace.yMin = h_1 - m_ppFacesMultiple[n]->rectFace.yMax;
				m_ppFacesMultiple[n]->rectFace.yMax = yMax;
			}
		}
		else if(m_nMirror == YS_ENGINE_MIRROR_NONE)
		{
			int w_1 = m_nWidthIn - 1;
			for (int n=0; n<m_numFaces; ++n)
			{
				int xMax = w_1 - m_ppFacesMultiple[n]->rectFace.xMin;
				int yMax = h_1 - m_ppFacesMultiple[n]->rectFace.yMin;
				m_ppFacesMultiple[n]->rectFace.xMin = w_1 - m_ppFacesMultiple[n]->rectFace.xMax;
				m_ppFacesMultiple[n]->rectFace.yMin = h_1 - m_ppFacesMultiple[n]->rectFace.yMax;
				m_ppFacesMultiple[n]->rectFace.xMax = xMax;
				m_ppFacesMultiple[n]->rectFace.yMax = yMax;
			}
		}
	}
}
