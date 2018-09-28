//
//  pkmac2.cpp
//  U3dTest
//
//  Created by xiaohui on 2017/1/11.
//  Copyright © 2017年 milook. All rights reserved.
//

#include "Interface.h"
#include <string>
MLFaceTracker* unity_tracker;
MaskModelPtsMapping mask;
using namespace std;
#define MAX_FACE 3
int faceNum=3;
int trackerFaceNum=0;
void delaunay(int pointNum, YSNode* points, int* faceNum, int* faceIdx);
int init(const char* path,int width,int height,int rotation,int mirror){
    unity_tracker = new MLFaceTracker();
    unity_tracker->SetMaxNumFace(min(MAX_FACE, faceNum));

    int format = YS_ENGINE_VIDEO_RGBA32;
    int error =unity_tracker->CreateEngine(path);
    error = unity_tracker->SetVideoFormat(width ,height ,format ,rotation ,mirror);
    return error;
}

stFaceInfo ** updateTrackingData(unsigned char *address,int size){
    int error=0;
    stFaceInfo **ppFace = unity_tracker->ProcessFrameMultipleFace(address, size, &trackerFaceNum);
    if(trackerFaceNum<=0){
        error=1;
        return NULL;
    }
    if(error==0){
        mask.MaskModelPtsFromTracker(ppFace[0]->pNodes,true);
    }
    return ppFace;
}

void MaskModelPtsFromTracker(YSNode* node , bool camera){
    mask.MaskModelPtsFromTracker(node,camera);
}
int getFaceNum(){
    return trackerFaceNum;
}

MLVector3* getMaskData(){
    return mask.m_vertices;
}

