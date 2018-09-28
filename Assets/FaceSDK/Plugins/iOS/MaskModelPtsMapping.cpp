//
//  MaskModelPtsMapping.cpp
//  MiloFaceTracker
//
//  Created by JiangTao on 2016/11/14.
//  Copyright © 2016年 chenyungui. All rights reserved.
//

#include "MaskModelPtsMapping.h"

//index corresponding to 78 tracker points
static int pIdxMappingTracker[] = {
    73, 75, 74, 92, 76, 77, 71, 66, 6, 13, 18, 24, 23, 41, 21, 22, 20,  // contour points from tracker (0 ... 16)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //brow points 17 ... 26 : need to fix
    1, 43, 3, 2,    // nose points 27 ... 30
    90, 91, 38, 40, 39,     // nosetril points 31 ... 35
    96, 95, 99, 100, 98, 97,    // left eye points 36 ... 41
    50, 49, 45, 46, 47, 48,     // right eye points 42 ... 47
    65, 93, 84, 4, 31, 42, 12, 33, 35, 7, 88, 86,   // mouth out points 48 ... 59
    0, 0, 0, 0, 0, 0,   // mouth inner points 60 ... 65 : need to fix
    83, 81, 28, 30,     // nose side points 66 ... 69
    78, 67, 62, 37, 9, 14, 25,  // head top points 70 ... 76
    0   // point 77 : need to fix
};

static void getBezierControlPt(YSNode nodeStart, YSNode nodeCurve, YSNode nodeEnd, YSNode &nodeControl)
{
    //distance
    float dx = nodeStart.x - nodeCurve.x;
    float dy = nodeStart.y - nodeCurve.y;
    float c1 = sqrt(dx * dx + dy * dy);
    dx = nodeEnd.x - nodeCurve.x;
    dy = nodeEnd.y - nodeCurve.y;
    float c2 = sqrt(dx * dx + dy * dy);
    float t = c1 / (c1 + c2);
    
    float b0 = (1 - t) * (1 - t);
    float b1 = t * (1 - t) * 2;
    float b2 = t * t;
    nodeControl.x = (nodeCurve.x - nodeStart.x * b0 - nodeEnd.x * b2) / b1;
    nodeControl.y = (nodeCurve.y - nodeStart.y * b0 - nodeEnd.y * b2) / b1;
}

static void getBezierInterPt(YSNode nodeStart, YSNode nodeEnd, YSNode nodeControl, float fRatio, float &xInter, float &yInter)
{
    float mt = 1 - fRatio;
    float a = mt * mt;
    float b = 2 * mt * fRatio;
    float c = fRatio * fRatio;
    
    xInter = nodeStart.x * a + nodeControl.x * b + nodeEnd.x * c;
    yInter = nodeStart.y * a + nodeControl.y * b + nodeEnd.y * c;
}

/**
 * line : Ax + By + C = 0
 */
static void getLineFrom2Pts(float x0, float y0, float x1, float y1, float &A, float &B, float &C)
{
    A = y0 - y1;
    B = x1 - x0;
    C = - y1 * B - x1 * A;
}

/**
 * line : Ax + By + C = 0
 * @param   fOff        : offset distance
 * @param   bClockwise  : right hand rule
 */
static void getParrallelLineWithOffset(float x0, float y0, float x1, float y1, float fOff, bool bClockwise, float &A, float &B, float &C)
{
    A = y0 - y1;
    B = x1 - x0;
    C = - y1 * B - x1 * A;
    
    if (bClockwise)
        C = C - fOff * sqrt(A * A + B * B);
    else
        C = C + fOff * sqrt(A * A + B * B);
}

/**
 * get vertical line from one point to a line
 */
static void getVerticalLineFromPointLine(float x0, float y0, float A, float B, float C, float &Av, float &Bv, float &Cv)
{
    Av = B;
    Bv = -A;
    Cv = A * y0 - B * x0;
}

/**
 * line 1 : A1x + B1y + C1 = 0
 * line 2 : A2x + B2y + C2 = 0
 * @return  : false if 2 line not cross
 */
static bool getCrossFrom2Lines(float A1, float B1, float C1, float A2, float B2, float C2, float &xCross, float &yCross)
{
    if (A1 * B2 == A2 * B1) //not cross
    {
        return false;
    }
    
    float fDiv = 1.0f / (A1 * B2 - A2 * B1);
    xCross = (B1 * C2 - B2 * C1) * fDiv;
    yCross = (A2 * C1 - A1 * C2) * fDiv;
    
    return true;
}

MaskModelPtsMapping::MaskModelPtsMapping(){}
MaskModelPtsMapping::~MaskModelPtsMapping(){}

void MaskModelPtsMapping::MaskModelPtsFromTracker(YSNode pNodesTrack[78], bool bFrontalCamera)
{
    // mapping some fixed points directly
    for (int i=0; i<17; ++i) {
        m_vertices[ pIdxMappingTracker[i] ].x = pNodesTrack[i].x;
        m_vertices[ pIdxMappingTracker[i] ].y = pNodesTrack[i].y;
    }
    for (int i=27; i<60; ++i) {
        m_vertices[ pIdxMappingTracker[i] ].x = pNodesTrack[i].x;
        m_vertices[ pIdxMappingTracker[i] ].y = pNodesTrack[i].y;
    }
    for (int i=66; i<78; ++i) {
        m_vertices[ pIdxMappingTracker[i] ].x = pNodesTrack[i].x;
        m_vertices[ pIdxMappingTracker[i] ].y = pNodesTrack[i].y;
    }
    
    // calculate inner mouth points
    GetInnerMouthPts(pNodesTrack);
    
    // outer eyes
    int lEyeOuterIndices[6] = {
        89, 69, 63, 64, 82, 70  //left eye outer in model
    };
    int lEyeInnerIndices[6] ={
        96, 95, 99, 100, 98, 97 // left eye inner in model
    };
    int rEyeOuterIndices[6] ={
        11, 10, 16, 36, 17, 29  //right eye outer in model
    };
    int rEyeInnerIndices[6]= {
        50, 49, 45, 46, 47, 48  // right eye inner in model
    };
    // calculate left eye outer offset points
    GetOuterEyePts(lEyeInnerIndices, lEyeOuterIndices, !bFrontalCamera);
    
    // calculate right eye outer offset points
    GetOuterEyePts(rEyeInnerIndices, rEyeOuterIndices, !bFrontalCamera);
    
    // calculate brow points
    // MARK: -
    // MARK: need to offset brows point to get up and low vertics position
    
    //left brow
    int lBrowIndices[10] ={
        111, 110, 109, 108, 107, 72, 68, 80, 79, 94
    };
    int lBrowTrackerIndices[5] ={
        17, 18, 19, 20, 21
    };
    
    float dx = pNodesTrack[36].x - pNodesTrack[39].x;
    float dy = pNodesTrack[36].y - pNodesTrack[39].y;
    float fOffsetBrowL = sqrt(dx * dx + dy * dy) * 0.15;
    GetBrowPts(pNodesTrack, lBrowIndices, lBrowTrackerIndices, fOffsetBrowL);
    
    //right brow
    int rBrowIndices[10] ={
        57, 58, 59, 60, 61, 44, 26, 27, 15, 19
    };
    int rBrowTrackerIndices[5]= {
        22, 23, 24, 25, 26
    };
    dx = pNodesTrack[42].x - pNodesTrack[45].x;
    dy = pNodesTrack[42].y - pNodesTrack[45].y;
    float fOffsetBrowR = sqrt(dx * dx + dy * dy) * 0.15;
    GetBrowPts(pNodesTrack, rBrowIndices, rBrowTrackerIndices, fOffsetBrowR);

    // calculate left inner face side points
    GetInnerFaceSidePtsLeft(pNodesTrack);

    // calculate right inner face side points
    GetInnerFaceSidePtsRight(pNodesTrack);
    
    // point 0
    GetBrowCenter(pNodesTrack);
}


/**
 
 original:
 
    v1   v2   v3
 v0                v4
    v7   v6   v5
 
 result:
 
        v2  v3  v4
    v1             v5
 v0                   v6
    v11            v7
        v10 v9  v8
 
 - parameter facePoints:       facePoints
 - parameter faceInnerIndices: faceInnerIndices described above
 
 - returns: return the values with interpolated values
 */
void MaskModelPtsMapping::GetInnerMouthPts(YSNode pNodesTrack[78])
{
    
    int pIdxFromTrack[] = {48, 60, 61, 62, 54, 63, 64, 65};
    int pIdxModel[] = {65, 104, 85, 5, 32, 54, 12, 55, 34, 8, 87, 105};
    
    //get bezier interpolation
    YSNode nodeTL, nodeTR, nodeBL, nodeBR;
    getBezierControlPt(pNodesTrack[ pIdxFromTrack[0] ], pNodesTrack[ pIdxFromTrack[1] ], pNodesTrack[ pIdxFromTrack[2] ], nodeTL);
    getBezierControlPt(pNodesTrack[ pIdxFromTrack[2] ], pNodesTrack[ pIdxFromTrack[3] ], pNodesTrack[ pIdxFromTrack[4] ], nodeTR);
    getBezierControlPt(pNodesTrack[ pIdxFromTrack[0] ], pNodesTrack[ pIdxFromTrack[7] ], pNodesTrack[ pIdxFromTrack[6] ], nodeBL);
    getBezierControlPt(pNodesTrack[ pIdxFromTrack[6] ], pNodesTrack[ pIdxFromTrack[5] ], pNodesTrack[ pIdxFromTrack[4] ], nodeBR);
    
    //get mouth inner points in model
    m_vertices[ pIdxModel[0] ].x = pNodesTrack[ pIdxFromTrack[0] ].x;
    m_vertices[ pIdxModel[0] ].y = pNodesTrack[ pIdxFromTrack[0] ].y;
    
    getBezierInterPt(pNodesTrack[pIdxFromTrack[0] ], pNodesTrack[pIdxFromTrack[2] ], nodeTL, 1.0f/3.0f, m_vertices[ pIdxModel[1] ].x, m_vertices[ pIdxModel[1] ].y);
    getBezierInterPt(pNodesTrack[pIdxFromTrack[0] ], pNodesTrack[pIdxFromTrack[2] ], nodeTL, 2.0f/3.0f, m_vertices[ pIdxModel[2] ].x, m_vertices[ pIdxModel[2] ].y);
    
    m_vertices[ pIdxModel[3] ].x = pNodesTrack[ pIdxFromTrack[2] ].x;
    m_vertices[ pIdxModel[3] ].y = pNodesTrack[ pIdxFromTrack[2] ].y;
    
    getBezierInterPt(pNodesTrack[pIdxFromTrack[2] ], pNodesTrack[pIdxFromTrack[4] ], nodeTR, 1.0f/3.0f, m_vertices[ pIdxModel[4] ].x, m_vertices[ pIdxModel[4] ].y);
    getBezierInterPt(pNodesTrack[pIdxFromTrack[2] ], pNodesTrack[pIdxFromTrack[4] ], nodeTR, 2.0f/3.0f, m_vertices[ pIdxModel[5] ].x, m_vertices[ pIdxModel[5] ].y);
    
    m_vertices[ pIdxModel[6] ].x = pNodesTrack[ pIdxFromTrack[4] ].x;
    m_vertices[ pIdxModel[6] ].y = pNodesTrack[ pIdxFromTrack[4] ].y;
    
    getBezierInterPt(pNodesTrack[pIdxFromTrack[6] ], pNodesTrack[pIdxFromTrack[4] ], nodeBR, 2.0f/3.0f, m_vertices[ pIdxModel[7] ].x, m_vertices[ pIdxModel[7] ].y);
    getBezierInterPt(pNodesTrack[pIdxFromTrack[6] ], pNodesTrack[pIdxFromTrack[4] ], nodeBR, 1.0f/3.0f, m_vertices[ pIdxModel[8] ].x, m_vertices[ pIdxModel[8] ].y);
    
    m_vertices[ pIdxModel[9] ].x = pNodesTrack[ pIdxFromTrack[6] ].x;
    m_vertices[ pIdxModel[9] ].y = pNodesTrack[ pIdxFromTrack[6] ].y;
    
    getBezierInterPt(pNodesTrack[pIdxFromTrack[0] ], pNodesTrack[pIdxFromTrack[6] ], nodeBL, 2.0f/3.0f, m_vertices[ pIdxModel[10] ].x, m_vertices[ pIdxModel[10] ].y);
    getBezierInterPt(pNodesTrack[pIdxFromTrack[0] ], pNodesTrack[pIdxFromTrack[6] ], nodeBL, 1.0f/3.0f, m_vertices[ pIdxModel[11] ].x, m_vertices[ pIdxModel[11] ].y);
    
}

//internal func offsetEyeOuterVertices(_ innerIndices:[Int], outerIndices:[Int])
void MaskModelPtsMapping::GetOuterEyePts(int pIdxInner[6], int pIdxOuter[6], bool bClockwise)
{
    //get bounds
    int xMinIn, xMaxIn, yMinIn, yMaxIn;
    xMinIn = xMaxIn = m_vertices[ pIdxInner[0] ].x;
    yMinIn = yMaxIn = m_vertices[ pIdxInner[0] ].y;
    for (int i=1; i<6; ++i)
    {
        if (xMinIn > m_vertices[ pIdxInner[i] ].x)
            xMinIn = m_vertices[ pIdxInner[i] ].x;
        else if (xMaxIn < m_vertices[ pIdxInner[i] ].x)
            xMaxIn = m_vertices[ pIdxInner[i] ].x;

        if (yMinIn > m_vertices[ pIdxInner[i] ].y)
            yMinIn = m_vertices[ pIdxInner[i] ].y;
        else if (yMaxIn < m_vertices[ pIdxInner[i] ].y)
            yMaxIn = m_vertices[ pIdxInner[i] ].y;
    }
    
    //offset value
    float offset = YS_MAX(yMaxIn - yMinIn, xMaxIn - xMinIn) * 0.2f;
    float xc = (xMinIn + xMaxIn) * 0.5f;
    float yc = (yMinIn + yMaxIn) * 0.5f;
    
    //offset to outer points
    
    //line 0
    float A1, B1, C1;
    float A2, B2, C2;
    
    //outer point 1
    getParrallelLineWithOffset(m_vertices[ pIdxInner[0] ].x, m_vertices[ pIdxInner[0] ].y, m_vertices[ pIdxInner[1] ].x, m_vertices[ pIdxInner[1] ].y, offset, bClockwise, A1, B1, C1);
    getParrallelLineWithOffset(m_vertices[ pIdxInner[1] ].x, m_vertices[ pIdxInner[1] ].y, m_vertices[ pIdxInner[2] ].x, m_vertices[ pIdxInner[2] ].y, offset, bClockwise, A2, B2, C2);
    bool bValid = getCrossFrom2Lines(A1, B1, C1, A2, B2, C2, m_vertices[ pIdxOuter[1] ].x, m_vertices[ pIdxOuter[1] ].y);
    if (!bValid) {
        //vertical line
        float Av, Bv, Cv;
        getVerticalLineFromPointLine(m_vertices[ pIdxInner[1] ].x, m_vertices[ pIdxInner[1] ].y, A2, B2, C2, Av, Bv, Cv);
        getCrossFrom2Lines(A1, B1, C1, Av, Bv, Cv, m_vertices[ pIdxOuter[1] ].x, m_vertices[ pIdxOuter[1] ].y);
    }
    
    //outer point 2
    getParrallelLineWithOffset(m_vertices[ pIdxInner[2] ].x, m_vertices[ pIdxInner[2] ].y, m_vertices[ pIdxInner[3] ].x, m_vertices[ pIdxInner[3] ].y, offset, bClockwise, A1, B1, C1);
    bValid = getCrossFrom2Lines(A1, B1, C1, A2, B2, C2, m_vertices[ pIdxOuter[2] ].x, m_vertices[ pIdxOuter[2] ].y);
    if (!bValid) {
        //vertical line
        float Av, Bv, Cv;
        getVerticalLineFromPointLine(m_vertices[ pIdxInner[2] ].x, m_vertices[ pIdxInner[2] ].y, A2, B2, C2, Av, Bv, Cv);
        getCrossFrom2Lines(A1, B1, C1, Av, Bv, Cv, m_vertices[ pIdxOuter[2] ].x, m_vertices[ pIdxOuter[2] ].y);
    }
    
    //outer point 4
    getParrallelLineWithOffset(m_vertices[ pIdxInner[3] ].x, m_vertices[ pIdxInner[3] ].y, m_vertices[ pIdxInner[4] ].x, m_vertices[ pIdxInner[4] ].y, offset, bClockwise, A1, B1, C1);
    getParrallelLineWithOffset(m_vertices[ pIdxInner[4] ].x, m_vertices[ pIdxInner[4] ].y, m_vertices[ pIdxInner[5] ].x, m_vertices[ pIdxInner[5] ].y, offset, bClockwise, A2, B2, C2);
    bValid = getCrossFrom2Lines(A1, B1, C1, A2, B2, C2, m_vertices[ pIdxOuter[4] ].x, m_vertices[ pIdxOuter[4] ].y);
    if (!bValid) {
        //vertical line
        float Av, Bv, Cv;
        getVerticalLineFromPointLine(m_vertices[ pIdxInner[4] ].x, m_vertices[ pIdxInner[4] ].y, A2, B2, C2, Av, Bv, Cv);
        getCrossFrom2Lines(A1, B1, C1, Av, Bv, Cv, m_vertices[ pIdxOuter[4] ].x, m_vertices[ pIdxOuter[4] ].y);
    }
    
    //outer point 5
    getParrallelLineWithOffset(m_vertices[ pIdxInner[5] ].x, m_vertices[ pIdxInner[5] ].y, m_vertices[ pIdxInner[0] ].x, m_vertices[ pIdxInner[0] ].y, offset, bClockwise, A1, B1, C1);
    bValid = getCrossFrom2Lines(A1, B1, C1, A2, B2, C2, m_vertices[ pIdxOuter[5] ].x, m_vertices[ pIdxOuter[5] ].y);
    if (!bValid) {
        //vertical line
        float Av, Bv, Cv;
        getVerticalLineFromPointLine(m_vertices[ pIdxInner[5] ].x, m_vertices[ pIdxInner[5] ].y, A2, B2, C2, Av, Bv, Cv);
        getCrossFrom2Lines(A1, B1, C1, Av, Bv, Cv, m_vertices[ pIdxOuter[5] ].x, m_vertices[ pIdxOuter[5] ].y);
    }
    
    //outer piont 0
    m_vertices[ pIdxOuter[0] ].x = 1.333333 * (m_vertices[ pIdxInner[0] ].x - xc) + xc;
    m_vertices[ pIdxOuter[0] ].y = 1.333333 * (m_vertices[ pIdxInner[0] ].y - yc) + yc;
    
    //outer piont 3
    m_vertices[ pIdxOuter[3] ].x = 1.333333 * (m_vertices[ pIdxInner[3] ].x - xc) + xc;
    m_vertices[ pIdxOuter[3] ].y = 1.333333 * (m_vertices[ pIdxInner[3] ].y - yc) + yc;

    /*
    let points = innerIndices.map{ self.vertices[$0].point }
    let bounds = points.bounds
    let distance = bounds.width * 0.2
    let polygon = Polygon2D(vertices: points)
    let expanded = polygon.expanded(distance, clockwise: false)
    for (i, index) in outerIndices.enumerated() {
        self.vertices[index].x = GLfloat(expanded.vertices[i].x)
        self.vertices[index].y = GLfloat(expanded.vertices[i].y)
    }
    
    // fix eye corner issue
    let center = bounds.center
    
    let lPoint = self.vertices[innerIndices[0]].point
    let rPoint = self.vertices[innerIndices[3]].point
    
    let lPosition = lPoint.scaled(around: center, value: 1.3333333)
    let rPosition = rPoint.scaled(around: center, value: 1.3333333)
    
    self.vertices[outerIndices[0]].x = GLfloat(lPosition.x)
    self.vertices[outerIndices[0]].y = GLfloat(lPosition.y)
    
    self.vertices[outerIndices[3]].x = GLfloat(rPosition.x)
    self.vertices[outerIndices[3]].y = GLfloat(rPosition.y)
    */
}

/**
 original:
 
 v0 -- v1 -- v2 -- v3 -- v4
 
 result:
 
 v0 -- v1 -- v2 -- v3 -- v4
 |
 |
 v5 -- v6 -- v7 -- v8 -- v9
 
 - parameter facePoints:  facePoints
 - parameter browIndices: brow tracker indices
 
 - returns: the offseted points
 */
//public func getBrowOffsettedPositions(_ facePoints:[CGPoint], browIndices:[Int], offset:CGFloat) -> [CGPoint]
void MaskModelPtsMapping::GetBrowPts(YSNode pNodesTrack[78], int pBrowIdx[10], int pBrowIdxTrack[5], float fOffset)
{
    //up lines
    float A_up[4], B_up[4], C_up[4];
    float A_down[4], B_down[4], C_down[4];
    for (int i=0; i<4; ++i)
    {
        getParrallelLineWithOffset(pNodesTrack[ pBrowIdxTrack[i] ].x, pNodesTrack[ pBrowIdxTrack[i] ].y, pNodesTrack[ pBrowIdxTrack[i+1] ].x, pNodesTrack[ pBrowIdxTrack[i+1] ].y, fOffset, false, A_up[i], B_up[i], C_up[i]);
        getParrallelLineWithOffset(pNodesTrack[ pBrowIdxTrack[i] ].x, pNodesTrack[ pBrowIdxTrack[i] ].y, pNodesTrack[ pBrowIdxTrack[i+1] ].x, pNodesTrack[ pBrowIdxTrack[i+1] ].y, fOffset, true, A_down[i], B_down[i], C_down[i]);
    }
    
    //point 0
    //vertical line
    float Av, Bv, Cv;
    getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[0] ].x, pNodesTrack[ pBrowIdxTrack[0] ].y, A_up[0], B_up[0], C_up[0], Av, Bv, Cv);
    getCrossFrom2Lines(A_up[0], B_up[0], C_up[0], Av, Bv, Cv, m_vertices[ pBrowIdx[0] ].x, m_vertices[ pBrowIdx[0] ].y);

    //point 1
    bool bValid = getCrossFrom2Lines(A_up[0], B_up[0], C_up[0], A_up[1], B_up[1], C_up[1], m_vertices[ pBrowIdx[1] ].x, m_vertices[ pBrowIdx[1] ].y);
    if (!bValid) {
        //vertical line
        getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[1] ].x, pNodesTrack[ pBrowIdxTrack[1] ].y, A_up[1], B_up[1], C_up[1], Av, Bv, Cv);
        getCrossFrom2Lines(A_up[1], B_up[1], C_up[1], Av, Bv, Cv, m_vertices[ pBrowIdx[1] ].x, m_vertices[ pBrowIdx[1] ].y);
    }

    //point 2
    bValid = getCrossFrom2Lines(A_up[1], B_up[1], C_up[1], A_up[2], B_up[2], C_up[2], m_vertices[ pBrowIdx[2] ].x, m_vertices[ pBrowIdx[2] ].y);
    if (!bValid) {
        //vertical line
        getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[2] ].x, pNodesTrack[ pBrowIdxTrack[2] ].y, A_up[2], B_up[2], C_up[2], Av, Bv, Cv);
        getCrossFrom2Lines(A_up[2], B_up[2], C_up[2], Av, Bv, Cv, m_vertices[ pBrowIdx[2] ].x, m_vertices[ pBrowIdx[2] ].y);
    }

    //point 3
    bValid = getCrossFrom2Lines(A_up[2], B_up[2], C_up[2], A_up[3], B_up[3], C_up[3], m_vertices[ pBrowIdx[3] ].x, m_vertices[ pBrowIdx[3] ].y);
    if (!bValid) {
        //vertical line
        getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[3] ].x, pNodesTrack[ pBrowIdxTrack[3] ].y, A_up[3], B_up[3], C_up[3], Av, Bv, Cv);
        getCrossFrom2Lines(A_up[3], B_up[3], C_up[3], Av, Bv, Cv, m_vertices[ pBrowIdx[3] ].x, m_vertices[ pBrowIdx[3] ].y);
    }

    //point 4
    //vertical line
    getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[4] ].x, pNodesTrack[ pBrowIdxTrack[4] ].y, A_up[3], B_up[3], C_up[3], Av, Bv, Cv);
    getCrossFrom2Lines(A_up[3], B_up[3], C_up[3], Av, Bv, Cv, m_vertices[ pBrowIdx[4] ].x, m_vertices[ pBrowIdx[4] ].y);

    //point 5
    //vertical line
    getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[0] ].x, pNodesTrack[ pBrowIdxTrack[0] ].y, A_down[0], B_down[0], C_down[0], Av, Bv, Cv);
    getCrossFrom2Lines(A_down[0], B_down[0], C_down[0], Av, Bv, Cv, m_vertices[ pBrowIdx[5] ].x, m_vertices[ pBrowIdx[5] ].y);
    
    //point 6
    bValid = getCrossFrom2Lines(A_down[0], B_down[0], C_down[0], A_down[1], B_down[1], C_down[1], m_vertices[ pBrowIdx[6] ].x, m_vertices[ pBrowIdx[6] ].y);
    if (!bValid) {
        //vertical line
        getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[1] ].x, pNodesTrack[ pBrowIdxTrack[1] ].y, A_down[1], B_down[1], C_down[1], Av, Bv, Cv);
        getCrossFrom2Lines(A_down[1], B_down[1], C_down[1], Av, Bv, Cv, m_vertices[ pBrowIdx[6] ].x, m_vertices[ pBrowIdx[6] ].y);
    }
    
    //point 7
    bValid = getCrossFrom2Lines(A_down[1], B_down[1], C_down[1], A_down[2], B_down[2], C_down[2], m_vertices[ pBrowIdx[7] ].x, m_vertices[ pBrowIdx[7] ].y);
    if (!bValid) {
        //vertical line
        getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[2] ].x, pNodesTrack[ pBrowIdxTrack[2] ].y, A_down[2], B_down[2], C_down[2], Av, Bv, Cv);
        getCrossFrom2Lines(A_down[2], B_down[2], C_down[2], Av, Bv, Cv, m_vertices[ pBrowIdx[7] ].x, m_vertices[ pBrowIdx[7] ].y);
    }
    
    //point 8
    bValid = getCrossFrom2Lines(A_down[2], B_down[2], C_down[2], A_down[3], B_down[3], C_down[3], m_vertices[ pBrowIdx[8] ].x, m_vertices[ pBrowIdx[8] ].y);
    if (!bValid) {
        //vertical line
        getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[3] ].x, pNodesTrack[ pBrowIdxTrack[3] ].y, A_down[3], B_down[3], C_down[3], Av, Bv, Cv);
        getCrossFrom2Lines(A_down[3], B_down[3], C_down[3], Av, Bv, Cv, m_vertices[ pBrowIdx[8] ].x, m_vertices[ pBrowIdx[8] ].y);
    }
    
    //point 9
    //vertical line
    getVerticalLineFromPointLine(pNodesTrack[ pBrowIdxTrack[4] ].x, pNodesTrack[ pBrowIdxTrack[4] ].y, A_down[3], B_down[3], C_down[3], Av, Bv, Cv);
    getCrossFrom2Lines(A_down[3], B_down[3], C_down[3], Av, Bv, Cv, m_vertices[ pBrowIdx[9] ].x, m_vertices[ pBrowIdx[9] ].y);
    
    /*
    let points = browIndices.map{ facePoints[$0] }
    let polyline = Polyline2D(vertices: points)
    
    let upLine = polyline.offseted(by: offset, clockwise: false)
    let downline = polyline.offseted(by: -offset, clockwise: false)
    
    return upLine.vertices + downline.vertices
     */
}

void MaskModelPtsMapping::GetInnerFaceSidePtsLeft(YSNode pNodesTrack[78])
{
    // [1] <-> 67
    // [2] <-> 66
    // [3] <-> 31
    int lFaceInnerIndices[] = {
        101, 106, 102, 103
    };
    
    m_vertices[ lFaceInnerIndices[0] ].x = pNodesTrack[1].x * 0.71 + pNodesTrack[67].x * 0.29;
    m_vertices[ lFaceInnerIndices[0] ].y = pNodesTrack[1].y * 0.71 + pNodesTrack[67].y * 0.29;
    
    m_vertices[ lFaceInnerIndices[1] ].x = pNodesTrack[1].x * 0.45 + pNodesTrack[67].x * 0.55;
    m_vertices[ lFaceInnerIndices[1] ].y = pNodesTrack[1].y * 0.45 + pNodesTrack[67].y * 0.55;
    
    m_vertices[ lFaceInnerIndices[2] ].x = pNodesTrack[2].x * 0.5 + pNodesTrack[66].x * 0.5;
    m_vertices[ lFaceInnerIndices[2] ].y = pNodesTrack[2].y * 0.5 + pNodesTrack[66].y * 0.5;
    
    m_vertices[ lFaceInnerIndices[3] ].x = pNodesTrack[3].x * 0.5 + pNodesTrack[31].x * 0.5;
    m_vertices[ lFaceInnerIndices[3] ].y = pNodesTrack[3].y * 0.5 + pNodesTrack[31].y * 0.5;
    
}

void MaskModelPtsMapping::GetInnerFaceSidePtsRight(YSNode pNodesTrack[78])
{
    // [15] <-> 68
    // [14] <-> 69
    // [13] <-> 35
    int rFaceInnerIndices[] = {
        56, 51, 52, 53
    };
    
    m_vertices[ rFaceInnerIndices[0] ].x = pNodesTrack[68].x * 0.55 + pNodesTrack[15].x * 0.45;
    m_vertices[ rFaceInnerIndices[0] ].y = pNodesTrack[68].y * 0.55 + pNodesTrack[15].y * 0.45;
    
    m_vertices[ rFaceInnerIndices[1] ].x = pNodesTrack[68].x * 0.29 + pNodesTrack[15].x * 0.71;
    m_vertices[ rFaceInnerIndices[1] ].y = pNodesTrack[68].y * 0.29 + pNodesTrack[15].y * 0.71;
    
    m_vertices[ rFaceInnerIndices[2] ].x = pNodesTrack[69].x * 0.5 + pNodesTrack[14].x * 0.5;
    m_vertices[ rFaceInnerIndices[2] ].y = pNodesTrack[69].y * 0.5 + pNodesTrack[14].y * 0.5;
    
    m_vertices[ rFaceInnerIndices[3] ].x = pNodesTrack[35].x * 0.5 + pNodesTrack[13].x * 0.5;
    m_vertices[ rFaceInnerIndices[3] ].y = pNodesTrack[35].y * 0.5 + pNodesTrack[13].y * 0.5;

}

// vertex 0
void MaskModelPtsMapping::GetBrowCenter(YSNode pNodesTrack[78])
{
    //line(27, 73) intersect with line(22, 72) to get vertices[0] position
 
    float A1, B1, C1, A2, B2, C2;
    getLineFrom2Pts(pNodesTrack[27].x, pNodesTrack[27].y, pNodesTrack[73].x, pNodesTrack[73].y, A1, B1, C1);
    getLineFrom2Pts(pNodesTrack[22].x, pNodesTrack[22].y, pNodesTrack[72].x, pNodesTrack[72].y, A2, B2, C2);

    getCrossFrom2Lines(A1, B1, C1, A2, B2, C2, m_vertices[0].x, m_vertices[0].y);
}


