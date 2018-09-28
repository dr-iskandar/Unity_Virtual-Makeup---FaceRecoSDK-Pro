/* Copyright (c) 2016 Shanghai Yingsui Inc. All right reserved. */
/**
 * @author Jiang Tao
 * MiLook Face Tracker wrapper class for Android MiLook SDK
 * ai@milook.me
 * @since 2016-10-17
 * @version 1.0
 */

#ifndef ML_FACE_TRACKER
#define ML_FACE_TRACKER

#include "YSEngineCommon.h"
#include "YSEngineFFTCommon.h"

#define NUM_FACE_MODEL_PT			6		//!< 3d face model points
#define NUM_FACE_SKIN_CONTOUR_PT	32		//!< number of face skin contour points
#define NUM_EYE_COLOR				3		//!< number eye color: R, G, B

/**
 * @brief Wrapper class to process camera data with facial tracking engine
 */
class YS_ENGINE_CPP_API MLFaceTracker
{
public:
	MLFaceTracker();
	virtual ~MLFaceTracker();

	/**
	 * @brief Create/release tracking engine
	 * @param	license		: full path of license file
	 * @return	YS_ENGINE_SUCCEED if succeed, other errors if failed
	 */
	int CreateEngine(const char *license);

	/**
	* @brief Release tracking engine
	*/
	void ReleaseEngine();

	/**
	* @brief Set video format for tracking engine
	* @param	nWidth		: width of resolution
	* @param	nHeight		: height of resolution
	* @param	nFormat		: video raw data format. @see enum EYSVideoFormat
	* @param	nRotation	: video data rotation angle. @see enum EYSVideoRotation
	* @param	nMirror		: video data mirror flag. @see enum EYSVideoMirror
	* @return	YS_ENGINE_SUCCEED if setting OK, other errors if setting failed
	*/
	int SetVideoFormat(int nWidth, int nHeight, int nFormat, int nRotation, int nMirror);

	/**
	* @brief Set minimum size of face in detection
	* @param	nSize		: minimum size of detected face
	*/
	void SetMinimumFaceSize(int nSize);

	/**
	* @brief Set max number of faces
	* @param	numFace		: max number of faces to detect/track
	*/
	void SetMaxNumFace(int numFace);

	/**
	* @brief Set interval for detection. 
	* @param	numFrames	: how many frames detection one time
	*/
	void SetDetectionInterval(int numFrames);

	/**
	* @brief Detect/track multiple faces
	* @param	ptrBuffer	: pointer to video frame raw data buffer
	* @param	szBuffer	: size of buffer in bytes
	* @param	numFaces	: number of tracked faces, 0 means no face tracked
	* @return	pointer to tracked faces if there's face
	*/
	stFaceInfo** ProcessFrameMultipleFace(void *ptrBuffer, int szBuffer, int *numFaces);

	/**
	* @brief Process each frame for tracking in real time
	* @param	ptrBuffer	: pointer to video frame raw data buffer
	* @param	sizeBuffer	: size of buffer in bytes
	* @return	YS_ENGINE_SUCCEED if tracking successfully, other errors when tracking failed
	*/
	int ProcessFrame(void *ptrBuffer, int sizeBuffer);

	/**
	* @brief Update face tracking result data
	* @param	pFeaturePoints	: tracked facial feature points
	* @param	pFacePose		: tracked face pose information
	* @param	pExpressions	: tracked facial expressions
	* @return	YS_ENGINE_SUCCEED if update succeed, else other errors
	*/
	int UpdateTrackingData(YSNode *pFeaturePoints, float *pFacePose, float *pExpressions);

	/**
	* @brief Update face/object tracking result data
	* @param	pFeaturePoints	: tracked facial feature points
	* @param	pFacePose		: tracked face pose information
	* @param	pExpressions	: tracked facial expressions
	* @param	p3dUserFace		: tracked user face's 3d model (estimated 6 nodes)
	* @param	rect			: tracked face rect
	* @return	YS_ENGINE_SUCCEED if update succeed, else other errors
	*/
	int UpdateTrackingData(YSNode *pFeaturePoints, float *pFacePose, float *pExpressions, YSNode *p3dUserFace, YSRect *rect);

	/**
	* @brief Get face skin contour points
	* @param	ptrBuffer		: pointer to video frame raw data buffer which used for facial tracking
	* @param	sizeBuffer		: size of buffer in bytes
	* @return	YS_ENGINE_SUCCEED if found skin contour successfully, other errors when failed
	*/
	int GetFaceSkinContour(void *ptrBuffer, int sizeBuffer);

	/**
	* @brief Get user 3d face model
	*/
	void Feedback3dUserFaceModel();

	/**
	* @brief Mapping frontal 3D coordination to 2D tracking coordination
	* @param	x	: [i] 3d coordinate X based on frontal model
	* @param	y	: [i] 3d coordinate Y
	* @param	z	: [i] 3d coordinate Z
	* @param	xT	: [o] projected 2d coordinate X on video image
	* @param	yT	: [o] projected 2d coordinate Y on video image
	*/
	void Mapping3D2Tracking(float x, float y, float z, float *xT, float *yT);

	/**
	* @brief Get face smoothing factors map image for face beautify
	* @param	nScaleMap	: [i] ratio of video frame size and map image size
	*/
	int GetFaceSmoothFactorMap(int nScaleMap);

private:

	//forbidden copy
	MLFaceTracker(const MLFaceTracker &tp);

	/**
	* @brief Rotate flip input buffer to gray pixel buffer for face detection based on setting
	* gray pixel buffer after rotation/flip is stored into member variable m_pBufferRotate
	* @param	ptrBuffer		: input pixel buffer data
	* @return	YS_ENGINE_SUCCEED if succeed, else other errors
	*/
	int RotateFlipBuffer(const void *ptrBuffer);

	/**
	* @brief Transform tracking points to original view
	* @param	pNodeRotated	: rotated nodes
	* @param	pNodeOriginal	: nodes in original view
	* @param	numPts			: number of points
	*/
	void RotateFlip2OriginalView( YSNode *pNodeRotated, YSNode *pNodeOriginal, int numPts);

	/**
	* @brief Transform face pose to original view
	* @param	pfPoseOriginal	: face pose in original view
	*/
	void FacePose_RotateFlip2OriginalView( float *pfPoseOriginal);

	/**
	* @brief Detect face based on position
	* @param	ptrBuffer		: input frame raw data
	* @param	sizeBuffer		: raw data size in bytes
	* @param	xPos			: x position
	* @param	yPos			: y position
	* @param	rectROI			: rect of interesting
	* @param	nSizeDetectMin	: minimum face size of detection
	*/
	int DetectFace(void *ptrBuffer, int sizeBuffer, int xPos, int yPos, YSRect rectROI, int nSizeDetectMin);

	/**
	* @brief Update face rect
	*/
	void UpdateFaceBoundingRectFromContour();

	/**
	* @brief Rotate / flip multiple face rect
	*/
	void RotateFlipMultipleFaceRect();

public:

	//face tracking results
	YSFace			m_Face;
	YSNode			m_FeaturePoints[NUM_MASK_MODEL_78];				//!< [o] allocated buffer to store the feature position data
	float			m_fFacePose[NUM_POSE_6];						//!< [o] 6 components: position x, y, z (scale), angle x, y, z
	float			m_fExpressions[EM_NUM_TOTAL];					//!< [o] 18 expressions + new expressions

	YSNode			m_pUser3dFaceModel[NUM_FACE_MODEL_PT];			//!< [o] 3d face model of tracking user
	YSNode			m_pFaceSkinContour[NUM_FACE_SKIN_CONTOUR_PT];	//!< [o] face skin contour points
	float			m_pAvgRGBLeftEye[NUM_EYE_COLOR];				//!< [o] left eye average color RGB
	float			m_pAvgRGBRightEye[NUM_EYE_COLOR];				//!< [o] right eye average color RGB

	unsigned char*	m_pMapFaceSmoothFactors;						//!< map image of face smoothing factor
	int				m_nWidthMap;									//!< width of map
	int				m_nHeightMap;									//!< height of map
	int				m_nScaleMap;									//!< scale ratio of video frame size and map size

private:

	//buffer
	unsigned char*  m_pBufferRotate;		//!< buffer stored rotated frame data
	bool			m_bAllocBufRot;			//!< indicate if m_pBufferRotate allocated memory

	//tracking
	bool		m_bInitEngine;				//!< indicate engine initialized or not
	int			m_nWidth;					//!< width of video resolution for processing (after rotation/flip)
	int			m_nHeight;					//!< height of video resolution for processing (after rotation/flip)
	int			m_nWidthIn;					//!< input width of video resolution
	int			m_nHeightIn;				//!< input height of video resolution
	int         m_nWidthstepIn;				//!< input width step of video frame buffer
	int 		m_nSizeBufferIn;			//!< size of input buffer
    int         m_nChannals;				//!< channel of input video frame
	int			m_nFormat;					//!< pixel format of input video frame
	int			m_nRotation;				//!< rotation of video frame needed for processing
	int			m_nMirror;					//!< mirror of video frame needed for processing
	bool		m_bHasFace;					//!< indicate if has face detected
	bool		m_bHasFFT;					//!< indicate if FFT succeed
	YSRect      m_rectROI;					//!< ROI rect for detection/tracking
	int			m_nSizeDetect;				//!< face detection size

    bool        m_bMask3d;					//!< 3d mask flag, default as enable
	YSRect		m_rectTracking;				//!< tracking rect

	//multiple face tracking
	int			m_numMaxFace;				//!< max number of faces detected/tracked
	int			m_numFrameCheckFace;		//!< number of frames for checking/detect face one time
	int 		m_nFrameNumFaceTracking;	//!< current frame no. after last face detection
	stFaceInfo	**m_ppFacesMultiple;		//!< tracked multiple faces
	int			m_numFaces;					//!< tracked face number
	int			m_numAllocatedFaces;		//!< allocated face number
};

#endif
