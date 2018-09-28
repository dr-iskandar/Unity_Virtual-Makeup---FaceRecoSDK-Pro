/* Copyright (c) 2015 Shanghai Yingsui Technology Co., Ltd. All right reserved. */

#ifndef _ENGINE_BTCOMMON_H_
#define _ENGINE_BTCOMMON_H_

#include "YSEngineCommon.h"

///enum warning info of different environment
enum EYSWarning
{
	YS_ENGINE_WARNING_NORMAL = 0,				//normal status
	YS_ENGINE_WARNING_NO_BODY = 1,			//no body inside camera view
	YS_ENGINE_WARNING_CLOSE_TO_CAMERA = 2,	//body too close to camera
	YS_ENGINE_WARNING_FAR_FROM_CAMERA = 3,	//body too far from camera
	YS_ENGINE_WARNING_CLOSE_TOP = 4,			//body too close to top, camera sight need to be higher
	YS_ENGINE_WARNING_CLOSE_BOTTOM = 5,		//body too close to bottom, camera sight need to be lower
	YS_ENGINE_WARNING_CLOSE_LEFT = 6,			//body too close to left edge, need to move to center, OR adjust camera
	YS_ENGINE_WARNING_CLOSE_RIGHT = 7,		//body too close to right edge, need to move to center, OR adjust camera
	YS_ENGINE_WARNING_BAD_IMAGE_QUALITY = 8,	//image quality is not good for body tracking
	YS_ENGINE_WARNING_LIGHTING_BRIGHT = 9,	//lighting condition is not good for body tracking, strong lighting
	YS_ENGINE_WARNING_LIGHTING_DARK = 10,		//too dark for body tracking
	YS_ENGINE_WARNING_BAD_BACKGROUND = 11,	//background color is not good for body tracking, maybe similar to body color
	YS_ENGINE_WARNING_MANY_PEOPLE = 12		//too many people in camera view
};

enum BODY_NODE_ID
{
	NODE_BODY_HEAD = 0,		//head center
	NODE_BODY_TORSO,		//torso node for cursor window mapping
	NODE_BODY_HAND_L,		//left hand
	NODE_BODY_HAND_R,		//right hand
	NODE_BODY_NECK,			//neck
	NODE_BODY_ROOT,			//upper body root
	NODE_BODY_SH_L,			//left shoulder
	NODE_BODY_SH_R,			//right shoulder
	NODE_BODY_ELBOW_L,		//left elbow
	NODE_BODY_ELBOW_R,		//right elbow
	NUM_BODY_NODES
};

enum FINGER_NODE_ID
{
	NODE_FINGER_TIPTHUMB	= 0,	//thumb fingertip
	NODE_FINGER_TIPINDEX,			//index fingertip
	NODE_FINGER_TIPMIDDLE,			//middle fingertip
	NODE_FINGER_TIPRING,			//ring fingertip
	NODE_FINGER_TIPLITTLE,			//little fingertip
	NODE_FINGER_ROOTTHUMB,			//thumb root
	NODE_FINGER_ROOTINDEX,			//index root
	NODE_FINGER_ROOTMIDDLE,			//middle root
	NODE_FINGER_ROOTRING,			//ring root
	NODE_FINGER_ROOTLITTLE,			//little root
	NODE_FINGER_POSITION,			//hand position
	NUM_FINGER_NODES
};

enum LOW_BODY_NODE_ID
{
	NODE_LOWBODY_HIPL	= 0,	//left node of hip joint					
	NODE_LOWBODY_HIPR,			//left node of hip joint
	NODE_LOWBODY_KNEEL,			//left node of knee
	NODE_LOWBODY_KNEER,			//right node of knee
	NODE_LOWBODY_FOOTL,			//left node of foot
	NODE_LOWBODY_FOOTR,			//right node of foot
	NUM_LOWBODY_NODES
};

typedef struct _YSBodyEx
{
	int			ID;				//body id.
	int			skeletalUnit;	//unit for skeletal body
	int			nOverlapping;	//overlapping with other people
	double		distance;		//distance between body and camera
	YSNode		bodyNodes[NUM_BODY_NODES];	//body nodes
} YSBodyEx;

typedef struct _YSHandEx
{
	int			ID;				//body id.
	int			state;			//hand state, reserved for future use
	double		palmSize;		//palm size of diameter
	YSRect 		rect;			//hand rect, reserved for future use
	YSNode		fingerNodes[NUM_FINGER_NODES];
}YSHandEx;

typedef struct _YSPeason
{
	YSBodyEx	body;
	YSHandEx	handLeft;
	YSHandEx	handRight;
	YSNode		lowBodys[NUM_LOWBODY_NODES];
}YSPeason;

//struct five fingertips for one hand
typedef struct _YSFingers
{
	double	palmSize;		//palm size of diameter
	YSNode	tipThumb;		//thumb fingertip
	YSNode	tipIndex;		//index fingertip
	YSNode	tipMiddle;		//middle fingertip
	YSNode	tipRing;		//ring fingertip
	YSNode	tipLittle;		//little fingertip
	YSNode	rootThumb;		//thumb root
	YSNode	rootIndex;		//index root
	YSNode	rootMiddle;		//middle root
	YSNode	rootRing;		//ring root
	YSNode	rootLittle;		//little root
} YSFingers;

//hand struct
typedef struct _YSHand
{
	YSFingers	fingers;		//fingertips position
	YSNode		node;			//hand position
	YSRect 		rect;			//hand rect, reserved for future use
	int			state;			//hand state, reserved for future use
} YSHand;

//body struct
typedef struct _YSBody
{
	int			ID;				//body id.
	int			skeletalUnit;	//unit for skeletal body
	double		distance;		//distance between body and camera
	YSNode 		face;			//face position
	YSNode		nodeTorso;		//torso node for cursor window mapping
	YSHand		handL;			//left hand
	YSHand		handR;			//right hand
	YSNode		nodeElbowL;		//left elbow
	YSNode		nodeElbowR;		//right elbow
	YSNode		nodeShoulderL;	//left shoulder
	YSNode		nodeShoulderR;	//right shoulder
	YSNode		nodeNeck;		//neck
	YSNode		nodeUpBody;		//joint UB
	YSNode		nodeRoot;		//upper body root

	YSNode		nodeHipL;		//left node of hip joint
	YSNode		nodeHipR;		//right node of hip joint
	YSNode		nodeKneeL;		//left node of knee
	YSNode		nodeKneeR;		//right node of knee
	YSNode		nodeFootL;		//left node of foot
	YSNode		nodeFootR;		//right node of foot
	int			nOverlapping;	//overlapping with other people
} YSBody;


//BT configuration struct
enum EModeBT
{
	TRACK_HEAD			= 0X0001,
	TRACK_HANDS			= 0X0002,
	TRACK_UPPER_BODY	= 0X0004,
	TRACK_FULL_BODY		= 0X0008,
	TRACK_FACE			= 0X0010,
	TRACK_FINGERS		= 0X0020,
	TRACK_PALM_STATUS	= 0X0040,
	TRACK_EYES			= 0X0080
};

typedef struct _YSBTConfig
{
	int	maxNum;					//max number of detected body, engine will return body data no more than max number
	int modeBT;					//mode of body tracking, default: TRACK_HEAD|TRACK_HANDS|TRACK_UPPER_BODY
} YSBTConfig;

//Engine configuration struct
typedef struct _YS_ENGINE_Config
{
	bool	bFFT;				//flag for activating facial feature tracking
	bool	bFR;				//flag for activating face comparison
	bool	bBT;				//flag for activating body tracking
	bool	bGR;				//flag for activating gesture recognition
	bool	bLegs;				//flag for activating legs tracking
	bool	bFingers;			//flag for activating fingertips tracking
} YS_ENGINE_Config;

//silhouette image mode
enum ESilhouetteMode
{
	SILHOUETTE_GRAY = 0,
	SILHOUETTE_ARGB,
	SILHOUETTE_CONTOUR_POINTS
};

/// enum operation mode of user
enum EYSOperationMode
{
	OPERATION_NONE = 0,			//no operation
	OPERATION_GESTURE = 1,		//gesture mode
	OPERATION_AIR_MOUSE = 2,	//air mouse mode
	OPERATION_TWO_HANDS = 3		//two hands mode
};

/// enum for palm mouse event when enable mouse mode
enum EYSPalmMouseEvent
{
	MOUSE_EVENT_DETECTION_FAIL = 0,		//no palm detected
	MOUSE_EVENT_PALM_MOVING = 1,		//detected palm, moving with palm open
	MOUSE_EVENT_PALM_WAVE = 2,			//wave palm open
	MOUSE_EVENT_CLICK = 3,				//click event: palm_fist_palm
	MOUSE_EVENT_PALM_TO_FIST = 4,		//from palm to fist (press button)
	MOUSE_EVENT_FIST_TO_PALM = 5,		//from fist to palm (release)
	MOUSE_EVENT_DRAG = 6,				//moving fist (palm close)
	MOUSE_EVENT_PUSH_IN = 7,			//palm push into camera
	MOUSE_EVENT_PULL_OUT = 8,			//palm pull back from camera
	MOUSE_EVENT_DOUBLE_CLICK = 9, 		//double click event: palm_fist_palm_fist_palm
	MOUSE_TWO_HANDS_ZOOM_OPEN = 10,		//two hands zoom in/out
	MOUSE_TWO_HANDS_ROTATE_CLOSE = 11	//two hands rotate
};

#endif	// _ENGINE_BTCOMMON_H_
