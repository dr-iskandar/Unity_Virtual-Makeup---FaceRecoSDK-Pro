//
//  MaskModelPtsMapping.hpp
//  MiloFaceTracker
//
//  Created by JiangTao on 2016/11/14.
//  Copyright © 2016年 chenyungui. All rights reserved.
//

#ifndef MaskModelPtsMapping_hpp
#define MaskModelPtsMapping_hpp
#include "YSEngineCommon.h"
#include <stdio.h>
#include <math.h>
const int NUM_MODEL_PT = 112;

struct MLVector3{
    float x;
    float y;
    float z;
};
class MaskModelPtsMapping
{
public:
    MaskModelPtsMapping();
    ~MaskModelPtsMapping();
    
    /**
     * get mask model points' pixel coordinates from tracked facial points, results saved in m_vertices
     * @param   pNodesTrack : tracked facial points
     */
    void MaskModelPtsFromTracker(YSNode pNodesTrack[78], bool bFrontalCamera = true);
    
    MLVector3 m_vertices[NUM_MODEL_PT];
    
private:
    void GetInnerMouthPts(YSNode pNodesTrack[78]);
    void GetOuterEyePts(int pIdxInner[6], int pIdxOuter[6], bool bClockwise = false);
    void GetBrowPts(YSNode pNodesTrack[78], int pBrowIdx[10], int pBrowIdxTrack[5], float fOffset);
    void GetInnerFaceSidePtsLeft(YSNode pNodesTrack[78]);
    void GetInnerFaceSidePtsRight(YSNode pNodesTrack[78]);
    void GetBrowCenter(YSNode pNodesTrack[78]);
};

#endif /* MaskModelPtsMapping_hpp */
