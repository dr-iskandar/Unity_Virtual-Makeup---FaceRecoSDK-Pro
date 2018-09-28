//
//  pkmac2.h
//  U3dTest
//
//  Created by xiaohui on 2017/1/11.
//  Copyright © 2017年 milook. All rights reserved.
//

#ifndef Interface_h
#define Interface_h
#include "MLFaceTracker.h"
#include "MaskModelPtsMapping.h"
extern "C" {
    stFaceInfo** updateTrackingData(unsigned char*,int);
    //typedef char* (*CALLBACK)(stFaceInfo **,int);
    int init(const char* path,int width,int height,int rotation,int mirror);
    MLVector3* getMaskData();
    int getFaceNum();
}

#endif
