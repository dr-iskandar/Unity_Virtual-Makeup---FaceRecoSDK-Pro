#ifndef _ENGINE_FFT_FFTCOMMON_H_
#define _ENGINE_FFT_FFTCOMMON_H_

#include "YSEngineCommon.h"

//tracking mode
#define FFT_26	0
#define FFT_22	1
#define FFT_15	2
#define FFT_8	3
#define FFT_68	4

//facial feature points number
#define NUM_FFT_26			26		// output 26 facial feature points

//face pose properties number
#define NUM_POSE_6			6		// size of face pose array

#define NUM_FACE_PROPERTY	6		// face property: x,y,z, ax,ay,az

// expression number
#define NUM_EXPRESSION		18		// number of expressions

//facial feature points number
#define NUM_FFT_68			68		// output 68 facial feature points

#define NUM_MASK_MODEL_78	78		// 78 points 3d mask model

//! 6 points of 3d face model: left/right eye corner, left/right nose side
//! corresponding to contour pointID: LEFT_EYE_36, LEFT_EYE_39, RIGHT_EYE_45, RIGHT_EYE_42, NOSE_NOSTRIAL_31, NOSE_NOSTRIAL_35
#define NUM_3D_FACE_6		6

// index (ID) of all nodes (28 feature points)
#define NODE_LP					0		// left pupil
#define NODE_RP					1		// right pupil
#define NODE_LBC				2		// left brow center
#define NODE_LBI				3		// left brow inner
#define NODE_LECO				4		// left eye corner outer
#define NODE_LECI				5		// left eye corner inner
#define NODE_LELU				6		// left eye lid upper
#define NODE_LELL				7		// left eye lid lower
#define NODE_LNS				8		// left nose side
#define NODE_LNT				9		// left nostril
#define NODE_LMC				10		// left mouth corner
#define NODE_LUL				11		// left upper lip
#define NODE_LLL				12		// left lower lip
#define NODE_RBC				13		// right brow center
#define NODE_RBI				14		// right brow inner
#define NODE_RECO				15		// right eye corner outer
#define NODE_RECI				16		// right eye corner inner
#define NODE_RELU				17		// right eye lid upper
#define NODE_RELL				18		// right eye lid lower
#define NODE_RNS				19		// right nose side
#define NODE_RNT				20		// right nostril
#define NODE_RMC				21		// right mouth corner
#define NODE_RUL				22		// right upper lip
#define NODE_RLL				23		// right lower lip
#define NODE_ULC				24		// upper lip center
#define NODE_LLC				25		// lower lip center
#define NODE_LBO				26		// left brow outer
#define NODE_RBO				27		// right brow outer

enum FACE_CONTOUR_POINT
{
	LEFT_CONTOUR_0,
	LEFT_CONTOUR_1,
	LEFT_CONTOUR_2,
	LEFT_CONTOUR_3,
	LEFT_CONTOUR_4,
	LEFT_CONTOUR_5,
	LEFT_CONTOUR_6,
	LEFT_CONTOUR_7,
	BOTTOM_CONTOUR_8,
	RIGHT_CONTOUR_9,
	RIGHT_CONTOUR_10,
	RIGHT_CONTOUR_11,
	RIGHT_CONTOUR_12,
	RIGHT_CONTOUR_13,
	RIGHT_CONTOUR_14,
	RIGHT_CONTOUR_15,
	RIGHT_CONTOUR_16,
	
	LEFT_EYE_BROW_17,
	LEFT_EYE_BROW_18,
	LEFT_EYE_BROW_19,
	LEFT_EYE_BROW_20,
	LEFT_EYE_BROW_21,

	RIGHT_EYE_BROW_22,
	RIGHT_EYE_BROW_23,
	RIGHT_EYE_BROW_24,
	RIGHT_EYE_BROW_25,
	RIGHT_EYE_BROW_26,

	NOSE_RIDGE_27,
	NOSE_RIDGE_28,
	NOSE_RIDGE_29,
	NOSE_RIDGE_30,

	NOSE_NOSTRIAL_31,
	NOSE_NOSTRIAL_32,
	NOSE_NOSTRIAL_33,
	NOSE_NOSTRIAL_34,
	NOSE_NOSTRIAL_35,

	LEFT_EYE_36,
	LEFT_EYE_37,
	LEFT_EYE_38,
	LEFT_EYE_39,
	LEFT_EYE_40,
	LEFT_EYE_41,

	RIGHT_EYE_42,
	RIGHT_EYE_43,
	RIGHT_EYE_44,
	RIGHT_EYE_45,
	RIGHT_EYE_46,
	RIGHT_EYE_47,

	MOUTH_OUTER_48,
	MOUTH_OUTER_49,
	MOUTH_OUTER_50,
	MOUTH_OUTER_51,
	MOUTH_OUTER_52,
	MOUTH_OUTER_53,
	MOUTH_OUTER_54,
	MOUTH_OUTER_55,
	MOUTH_OUTER_56,
	MOUTH_OUTER_57,
	MOUTH_OUTER_58,
	MOUTH_OUTER_59,

	MOUTH_INNER_60,
	MOUTH_INNER_61,
	MOUTH_INNER_62,
	MOUTH_INNER_63,
	MOUTH_INNER_64,
	MOUTH_INNER_65,
	MOUTH_INNER_66,
	MOUTH_INNER_67
};

//! Expression index
#define EX_E_CLOSE_SYM		0		// EX1 - Symmetric eye close
#define EX_E_CLOSE_R		1		// EX2 - Right eye close
#define EX_E_CLOSE_L		2		// EX3 - Left eye close
#define EX_E_OPEN_SYM		3		// EX4 - Symmetric wide eye open
#define EX_B_RAISE_SYM		4		// EX5 - Symmetric eyebrow raise
#define EX_B_RAISE_R		5		// EX6 - Right eyebrow raise
#define EX_B_RAISE_L		6		// EX7 - Left eyebrow raise
#define EX_B_FURROW_SYM		7		// EX8 - Symmetric eyebrow furrow
#define EX_M_AH				8		// EX9 - Ah-shape mouth open
#define EX_M_DIS			9		// EX10 - Disgusted mouth shape
#define EX_M_DOWN			10		// EX11 - Downward displacement of the mouth
#define EX_M_OH				11		// EX12 - Oh-shaped mouth
#define EX_M_EH				12		// EX13 - Eh-shaped mouth
#define EX_M_CLOSE_SMILE	13		// EX14 - Mouth-closed smile
#define EX_M_OPEN_SMILE		14		// EX15 - Mouth-open smile
#define EX_M_FROWN			15		// EX16 - Frown mouth shape
#define EX_M_PULL_RIGHT		16		// EX17 - Pull of the right mouth corner
#define EX_M_PULL_LEFT		17		// EX18 - Pull of the left mouth corner

//! neutral expressions
enum EYSNeutralExpression
{
	EM_N_HAPPY		= 0,		//replace EX_E_CLOSE_SYM
	EM_N_SAD		= 3,		//replace EX_E_OPEN_SYM
	EM_N_ANGRY		= 4,		//replace EX_B_RAISE_SYM
	EM_N_DISGUST	= 9,		//replace EX_M_DIS
	EM_N_FEAR		= 10,		//replace EX_M_DOWN
	EM_N_SURPRISE	= 15,		//replace EX_M_FROWN
	EM_N_EYE_BLINK	= 18,		//could be left/right eye blink, OR both eye blink
	EM_N_TALK		= 19,		//mouth open/close continuously
	EM_N_KISS		= 20,		//small mouth OH
	EM_NUM_TOTAL	= 21
};

//! FFT configuration struct
typedef struct Mobi_FFT_Config
{
	int		modeFFT;
	float	confidenceT;		//!< confidence threshold, range 0 - 1.0f
}FFTConfig;

//! video info for tracking face
struct stVideoInfo
{
	int		nWidthInput;		//!< input image width
	int		nHeightInput;		//!< input image height
	int		nVideoFormat;		//!< video/pixel format
	int		nRotation;			//!< rotation angle of image
	int		nFlip;				//!< flip setting of image
	int		nWidthRot;			//!< width after rotation/flip
	int		nHeightRot;			//!< height after rotation/flip
};

//! face info in tracking
struct stFaceInfo
{
	int			nIdFace;						//!< face id
	YSRect		rectFace;						//!< face rect
	float		fEyeDistance;					//!< distance of two pupils
	float		fConfidence;					//!< confidence value
	YSNode		nodeLP;							//!< left pupil point
	YSNode		nodeRP;							//!< right pupil point
	YSNode		pNodes[NUM_MASK_MODEL_78];		//!< 78 facial feature points
	float		pPose[NUM_POSE_6];				//!< face pose: Position: x, y, z, Euler angles: angleX, angleY, angleZ
	float		pExpressions[EM_NUM_TOTAL];		//!< facial expressions
	YSNode		p3dUserFace[NUM_3D_FACE_6];		//!< 3d user face model
	int			nStatus;						//!< status are: 0 - FD result; 1 - FFD result; 2 - FFT result
};

//! face status in stFaceInfo
enum EYSFaceStatus
{
	YS_FS_FD	= 0,		//!< FD succeed
	YS_FS_FFD	= 1,		//!< FFD succeed
	YS_FS_FFT	= 2,		//!< FFT succeed
	YS_FS_ERR	= -1		//!< unknown
};

#endif	//_ENGINE_FFT_FFTCOMMON_H_
