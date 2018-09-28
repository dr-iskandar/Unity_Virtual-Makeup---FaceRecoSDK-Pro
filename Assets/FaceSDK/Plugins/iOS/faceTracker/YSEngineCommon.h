/* Copyright (c) 2015 Shanghai Yingsui Technology Co., Ltd. All right reserved. */

#ifndef _ENGINE_COMMON_HEADER_
#define _ENGINE_COMMON_HEADER_

#include "platform.h"

#ifndef SVN_ID
#define SVN_ID $Revision$
#endif

/* Define NULL pointer value */
#ifndef NULL
	#ifdef  __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif


#define		YS_ENGINE_TRACKING_DEFAULT		0x0000		//Head tracking is base
#define		YS_ENGINE_TRACKING_HAND			0x0001		//hand tracking 
#define		YS_ENGINE_TRACKING_BODY			0x0002		//Body tracking
#define		YS_ENGINE_TRACKING_AIRMOUSE		0x0004		//AIR MOUSE tracking
#define		YS_ENGINE_TRACKING_FINGERS		0x0008		//fingers tracking
#define		YS_ENGINE_TRACKING_FACEREG		0x0010		//face register
#define		YS_ENGINE_TRACKING_FACECOMPARE	0x0020		//Face compare tracking
#define		YS_ENGINE_TRACKING_AGEGENDER	0x0040		//AGE GENDER tracking
#define		YS_ENGINE_TRACKING_FACEPOSE		0x0080		//FACE POSE tracking
#define		YS_ENGINE_TRACKING_LEGS			0x0100		//LEGS tracking


// error definition
enum EYSError 
{
	YS_ENGINE_SUCCEED			= 0,	// succeed
	YS_ENGINE_DETECT_FAIL		= 1,	// detection fail
	YS_ENGINE_NO_FACE			= 2,	// no face can be found
	YS_ENGINE_BAD_IMAGE			= 3,	// input image quality is bad
	YS_ENGINE_BIG_ANGLE_X		= 4,	// angle x of face is big
	YS_ENGINE_BIG_ANGLE_Y		= 5,	// angle y of face is big
	YS_ENGINE_BIG_ANGLE_Z		= 6,	// angle z of face is big
	YS_ENGINE_BIG_FACE			= 7,	// face size is too big
	YS_ENGINE_SMALL_FACE		= 8,	// face size is too small
	YS_ENGINE_CLOSE_TO_EDGE		= 9,	// face is close to the image edge
	YS_ENGINE_TRACK_FAIL		= 10,	// track face failed
	YS_ENGINE_OUT_OF_MEMORY		= 11,	// out of memory
	YS_ENGINE_INVALID_ARG		= 12,	// invalid input argument
	YS_ENGINE_NOT_IMPLEMENTED	= 13,	// not implemented
	YS_ENGINE_FILE_NOT_FOUND	= 14,	// file not found
	YS_ENGINE_INVALID_LICENSE	= 15,	// invalid license
	YS_ENGINE_STILL_FRAME		= 16	// still frame
};

//image color mode
enum EYSVideoFormat 
{
	YS_ENGINE_VIDEO_GREY			= 0,	//8 bits gray 
	YS_ENGINE_VIDEO_RGB24			= 1,	//24 bits rgb 
	YS_ENGINE_VIDEO_RGBA32			= 2,	//32 bits rgba 
	YS_ENGINE_VIDEO_NV21			= 3,	//yuv420sp_NV21
	YS_ENGINE_VIDEO_YUYV422packed	= 4,	//yuyv422packed
	YS_ENGINE_VIDEO_NV12			= 5		//yuv420sp_NV12
};

//video/image resolution
enum EYSResolution
{
	YS_ENGINE_RES_160X120		= 0,	//160 X 120
	YS_ENGINE_RES_176X144		= 1,	//176 X 144, QCIF
	YS_ENGINE_RES_320X240		= 2,	//320 X 240, Quarter-VGA (QVGA)
	YS_ENGINE_RES_352X288		= 3,	//352 X 288, CIF
	YS_ENGINE_RES_640x480		= 4,	//640 X 480, Video-Graphics-Array (VGA)
	YS_ENGINE_RES_800x600		= 5,	//800 X 600, Super-Video-Graphics-Array (SVGA)
	YS_ENGINE_RES_1024X768		= 6,	//1024 X 768, eXtended-VGA (XGA)
	YS_ENGINE_RES_1600X1200		= 7,	//1600 X 1200, Ultra-VGA (UGA)
	YS_ENGINE_RES_720P			= 8,	//1280 X 720
	YS_ENGINE_RES_1080P			= 9,	//1920 X 1080, HDTV High-Definition-TV
};

//video mirror flag
enum EYSVideoMirror
{
	YS_ENGINE_MIRROR_NONE		= 0,		//no flip
	YS_ENGINE_MIRROR_X			= 1,		//flip up-down
	YS_ENGINE_MIRROR_Y			= 2,		//flip left-right
	YS_ENGINE_MIRROR_XY			= 3			//flip left-right and up-down
};

//video rotation flag
enum EYSVideoRotation
{
	YS_ENGINE_ROTATION_NONE		= 0,		//no rotation
	YS_ENGINE_ROTATION_CLK_90	= 90,		//rotation clock-wise 90 degree
	YS_ENGINE_ROTATION_CLK_180	= 180,		//rotation clock-wise 180 degree
	YS_ENGINE_ROTATION_CLK_270	= 270		//rotation clock-wise 270 degree
};

// image quality / lighting condition
enum EYSFrameQuality
{
	YS_ENGINE_FRAME_NORMAL		= 0,		// image quality good enough for processing
	YS_ENGINE_FRAME_DARK		= 1,		// image is dark than normal
	YS_ENGINE_FRAME_BRIGHT		= 2,		// image is bright than normal
	YS_ENGINE_FRAME_BIG_DARK	= 3,		// image too dark, means lighting too lower
	YS_ENGINE_FRAME_BIG_BRIGHT	= 4,		// image too bright, means over lighting condition
};

// frame motion status
enum EYSFrameMotionStatus
{
	YS_ENGINE_FRAME_STILL			= 0,		// totally still frame with last frame
	YS_ENGINE_FRAME_NO_MOTION		= 1,		// normal frame but no motion
	YS_ENGINE_FRAME_SMALL_MOTION	= 2,		// normal frame with small motion
	YS_ENGINE_FRAME_BIG_MOTION		= 3,		// normal frame with big motion
	YS_ENGINE_FRAME_EXPOSURE		= 4			// exposure frame, means lighting is changing, and image quality not good
};

enum EYSEyeStatus
{
	YS_ENGINE_EYE_CLOSE			= 0,		// eye is close
	YS_ENGINE_EYE_OPEN			= 1,		// eye is open
	YS_ENGINE_EYE_UNKNOWN		= 2			// unknown
};

// The maximum number for tracking
enum EYSMaxBody
{
	YS_ENGINE_BODY_NONE			= 0,
	YS_ENGINE_BODY_ONE			= 1,		// a people body for tracking
	YS_ENGINE_BODY_TWO			= 2,		// two body
};


// typedef
// integer rectangle in an image
typedef struct _YSRect
{ 
	int 	xMin;			/* x position top-left corner */
	int 	yMin;			/* y position top-left corner */
	int 	xMax;			/* x position bottom-right corner */
	int 	yMax;			/* y position bottom-right corner */
} YSRect;

// node (feature point)
typedef struct _YSNode
{
	float	x;				/* x position */
	float	y;				/* y position */
	float	z;				/* z position, future use */
	int		id;				/* node ID */
	float	confidence;		/* confidence value */
} YSNode;

// face, defined by the middle point of two pupils, eye distance and the pose information
typedef struct _YSFace
{
	YSRect		rectFace;			// rect area of face
	YSNode		nodeLP;				// node of left pupil
	YSNode		nodeRP;				// node of right pupil
	float		x;					// x position
	float		y;					// y position
	float		eyeDistance;		// distance of two pupils
	float		confidence;			// confidence value
	float		pose[3];			// Euler angles: angleX, angleY, angleZ, future use
} YSFace;

typedef struct _YSObject
{
	YSRect		rectObject;			//rect area of object
	float		confidence;			//confidence value
	int			label;				//type id
	char		typeName[20];		//type name
}YSObject;

#endif // _ENGINE_COMMON_HEADER_
