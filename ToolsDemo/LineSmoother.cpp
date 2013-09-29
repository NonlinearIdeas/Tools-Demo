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

//#define DEBUG_LINE_SMOOTHER


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


void LineSmoother::ProcessNewPoint()
{
   if(_orgPoints.size() < 1)
   {  // Nothing to do.
      return;
   }
   uint32 newPointIndex = _orgPoints.size()-1;
#ifdef DEBUG_LINE_SMOOTHER
   _orgPoints[newPointIndex].widthPixels = 1.0*newPointIndex;
#endif
   CalculateVelocities(newPointIndex);
   CalculateWidths(newPointIndex);
   CalculateSmoothPoints(newPointIndex);
}

// Given a new original point at newPointIndex, calculate a new
// set of smoothed points and add them to the smoothPoints list.
void LineSmoother::CalculateSmoothPoints(uint32 newPointIndex)
{
   
}

float Median(float a, float b, float c)
{
   if(a >= b && a <= c)
      return a;
   if(a >= c && a <= b)
      return a;
   if(b >= a && b <= c)
      return b;
   if(b >= c && b <= a)
      return b;
   return c;
}

// Estimate the velocity of the new point relative to previous points
// in points/second.
void LineSmoother::CalculateVelocities(uint32 newPointIndex)
{

   if(newPointIndex == 0)
   {  // Must be the first point.
      _orgPoints[newPointIndex].pointsPerSecond = 0.0f;
      _orgPoints[newPointIndex].pointsPerSecondRaw = 0.0f;
   }
   else
   {
      const float PPS_MIN = 100.0f;
      const float PPS_MAX = 3000.0f;
      const float TS_MIN = 0.005f;  //  5 ms
      const float TS_MAX = 0.020f;  // 20 ms
      
      ORIGINAL_POINT& p0 = _orgPoints[newPointIndex-1];
      ORIGINAL_POINT& p1 = _orgPoints[newPointIndex];
      
      float dist = ccpDistance(p0.point, p1.point);
      float dt = clampf(p1.timestamp-p0.timestamp,TS_MIN,TS_MAX);
      
      float ppsRaw = dist/dt;
      p1.pointsPerSecondRaw = ppsRaw;
      // Do some median filtering on the point velocities to reject spikes
      if(newPointIndex == 1)
      {  // Must be the second point.
         ppsRaw = Median(ppsRaw, ppsRaw, p0.pointsPerSecondRaw);
      }
      else
      {
         ppsRaw = Median(ppsRaw, p0.pointsPerSecondRaw, _orgPoints[newPointIndex-2].pointsPerSecondRaw);
      }
      p1.pointsPerSecond = clampf(ppsRaw,PPS_MIN,PPS_MAX);
   }
}

// Calculate velocities for the original new point at newPointIndex.
void LineSmoother::CalculateWidths(uint32 newPointIndex)
{
   const float WIDTH_MIN = 0.25f;
   const float WIDTH_MAX = 10.0f;
   
   if(newPointIndex == 0)
   {
      _orgPoints[newPointIndex].widthPixels = WIDTH_MIN;
   }
   else
   {
      const float WIDTH_EPSILON = 0.25;
      const float GROWTH_FACTOR = 0.10;
      
      ORIGINAL_POINT& p0 = _orgPoints[newPointIndex-1];
      ORIGINAL_POINT& p1 = _orgPoints[newPointIndex];
      
      // Width for the current point.
      p1.widthPixels = p0.widthPixels;
      float widthEstimate = clampf(p1.pointsPerSecond/100.0f, WIDTH_MIN, WIDTH_MAX);
      float deltaWidthEstmate = widthEstimate-p1.widthPixels;
      //      CCLOG("width:%f, widthEstimate:%f, delta:%f",p1.widthPixels,widthEstimate,deltaWidthEstmate);
      if(fabs(deltaWidthEstmate) > WIDTH_EPSILON)
      {  // The new estimate is far enough away to warrant a change.
         p1.widthPixels += deltaWidthEstmate*GROWTH_FACTOR;
      }
      p1.widthPixels = clampf(p1.widthPixels, WIDTH_MIN, WIDTH_MAX);
   }
}


void LineSmoother::Reset()
{
   _orgPoints.clear();
   _smoothPoints.clear();
   _lastSmoothPointIndex = 0;
}

void LineSmoother::LineBegin(const CCPoint& point, double timestamp)
{
   // Clear out all data when a new line begins.
   Reset();
   
   ORIGINAL_POINT op;
   op.Init(point, timestamp, LP_BEGIN, ccp(0.0f,0.0f));
   _orgPoints.push_back(op);
   ProcessNewPoint();
}

void LineSmoother::LineContinue(const CCPoint& point, double timestamp)
{
   // Verify we are in a line.  This means the last
   // point loaded cannot be the end.
   assert(_orgPoints.size() > 0 &&
          _orgPoints[_orgPoints.size()-1].position != LP_END);
   
   ORIGINAL_POINT op;
   op.Init(point, timestamp, LP_CONTINUE, ccp(0.0f,0.0f));
   _orgPoints.push_back(op);
   ProcessNewPoint();
}


void LineSmoother::LineEnd(const CCPoint& point, double timestamp)
{
   // Verify we are in a line.  We are NOT checking
   // if the last point was a continue, so we could have
   // a 2-point line here, which will may throw off the
   // smoothing algorithm.
   assert(_orgPoints.size() > 0 &&
          _orgPoints[_orgPoints.size()-1].position != LP_END);
   ORIGINAL_POINT op;
      
   op.Init(point, timestamp, LP_END, ccp(0.0f,0.0f));
   _orgPoints.push_back(op);
   ProcessNewPoint();
#ifdef DEBUG_LINE_SMOOTHER
   // Dump the original point data for the line.
   CCLOG("--------------------------------------------------");
   /*
   for(int idx = 0; idx < _orgPoints.size(); idx++)
      _orgPoints[idx].Dump(idx);
    */
   CCLOG("--------------------------------------------------");
   CCLOG("Total Points: ORG:(%lu), SMOOTHED:(%lu)",
         _orgPoints.size(),_smoothPoints.size());
   CCLOG("--------------------------------------------------");
   for(int idx = 0; idx < _smoothPoints.size(); idx++)
      _smoothPoints[idx].Dump(idx);
#endif
}

LineSmoother::LineSmoother()
{
   
}

LineSmoother::~LineSmoother()
{
   Reset();
}
