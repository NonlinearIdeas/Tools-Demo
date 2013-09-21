/********************************************************************
 * File   : LineSmoother.cpp
 * Project: ToolsDemo
 *
 ********************************************************************
 * Created on 9/21/13 By Nonlinear Ideas Inc.
 * Copyright (c) 2013 Nonlinear Ideas Inc. All rights reserved.
 ********************************************************************
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software. If you use this
 *    software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and
 *    must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#include "LineSmoother.h"


/* Calculate the Hermite Spline at a given value of t, where t is drawn from [0,1].
 * Outputs: Interpolated point.
 * Inputs:  t - parametric value.
 *          p0, p1 - Start and end points of orginal data.
 *          m0, m1 - Tangent vector at the start and end point.
 *
 */
CCPoint LineSmoother::HermiteSpline(float t, CCPoint p0, CCPoint p1, CCPoint m0, CCPoint m1)
{
   float tSq = t*t;
   float tCube = t*t*t;
   CCPoint b0 = ccpMult(p0,2*tCube - 3*tSq + 1);
   CCPoint b1 = ccpMult(m0,tCube - 2*tSq + t);
   CCPoint b2 = ccpMult(p1,3*tSq - 2*tCube);
   CCPoint b3 = ccpMult(m1,tCube - tSq);
   
   CCPoint result(b0.x+b1.x+b2.x+b3.x,b0.y+b1.y+b2.y+b3.y);
   /*
    if(t == 0.0)
    CCLOG("Smoothing new point set.");
    CCLOG("HermiteSpline: t:%4.5f p0:(%f,%f) p1:(%f,%f) m0:(%f,%f) m1:(%f,%f) result:(%f,%f)",
    t,
    p0.x,p0.y,
    p1.x,p1.y,
    m0.x,m0.y,
    m1.x,m1.y,
    result.x,result.y
    );
    */
   return result;
}


/* New points are only added for LP_CONTINUE or
 * LP_END points.
 *
 * If the previous point was a begin, use this
 * point to estimate the tangent and set it.
 *
 * If the last point is an end, use the previous
 * point to estimate the tangent and set it.
 *
 * This function must be overriden in derived classes
 * to provide differnt types of smoothing.
 */
void LineSmoother::ProcessNewPoint()
{
   assert(false);
}

void LineSmoother::Reset()
{
   _orgPoints.clear();
   _smoothPoints.clear();
}

void LineSmoother::LineBegin(const CCPoint& point)
{
   // Clear out all data when a new line begins.
   Reset();
   
   ORIGINAL_POINT op;
   op.point = point;
   op.timestamp = 0.0;
   op.tangent = ccp(0.0,0.0);
   op.position = LP_BEGIN;
   op.smoothedStartIndex = _smoothPoints.size();
   _orgPoints.push_back(op);
   _stopwatch.Start();
   ProcessNewPoint();
}

void LineSmoother::LineContinue(const CCPoint& point)
{
   // Verify we are in a line.  This means the last
   // point loaded cannot be the end.
   assert(_orgPoints.size() > 0 &&
          _orgPoints[_orgPoints.size()-1].position != LP_END);
   
   ORIGINAL_POINT op;
   op.point = point;
   op.position = LP_CONTINUE;
   op.timestamp = _stopwatch.GetSeconds();
   op.tangent = ccp(0.0,0.0);
   op.smoothedStartIndex = _smoothPoints.size();
   _orgPoints.push_back(op);
   ProcessNewPoint();
}

void LineSmoother::LineEnd(const CCPoint& point)
{
   // Verify we are in a line.  We are NOT checking
   // if the last point was a continue, so we could have
   // a 2-point line here, which will may throw off the
   // smoothing algorithm.
   assert(_orgPoints.size() > 0 &&
          _orgPoints[_orgPoints.size()-1].position != LP_END);
   ORIGINAL_POINT op;
   op.point = point;
   op.position = LP_END;
   op.timestamp = _stopwatch.GetSeconds();
   op.tangent = ccp(0.0,0.0);
   op.smoothedStartIndex = _smoothPoints.size();
   _orgPoints.push_back(op);
   ProcessNewPoint();
}

LineSmoother::LineSmoother()
{
   
}

LineSmoother::~LineSmoother()
{
   Reset();
}
