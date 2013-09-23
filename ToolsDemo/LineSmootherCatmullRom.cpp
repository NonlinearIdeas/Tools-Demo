/********************************************************************
 * File   : LineSmootherCatmullRom.cpp
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

#include "LineSmootherCatmullRom.h"
#include "MathUtilities.h"

void LineSmootherCatmullRom::CalculateSmoothPoints(uint32 newPointIndex)
{
   const float pixelsPerTick = 2.0;
   
   vector<ORIGINAL_POINT>& orgPoints = GetOriginalPoints();
   vector<SMOOTHED_POINT>& smoothPoints = GetSmoothedPoints();
   
   if(orgPoints.size() < 3)
   {
      return;
   }
   
   if(newPointIndex == 2)
   {  // Must mean we have only 3 points in there.
      // So the first point is the starting point.
      ORIGINAL_POINT& p2 = orgPoints[newPointIndex];
      ORIGINAL_POINT& p1 = orgPoints[newPointIndex-1];
      ORIGINAL_POINT& p0 = orgPoints[newPointIndex-2];
      
      assert(p0.position == LP_BEGIN);
      p0.tangent = ccpSub(p1.point, p0.point);
      p1.tangent = ccpMult(ccpSub(p2.point, p0.point),0.5f);
      
      // Fill in the curve between the first point and the second.
      float distPixels = ccpDistance(p1.point, p0.point);
      int ticks = MAX(4, distPixels/pixelsPerTick);
      double dt = 1.0/(ticks);
      SMOOTHED_POINT smPt;
      smPt.position = LP_CONTINUE;
      for(int idx = 0; idx < ticks; idx++)
      {
         float time = idx*dt;
         smPt.point = HermiteSpline(time, p0.point, p1.point, p0.tangent, p1.tangent);
         smPt.widthPixels = MathUtilities::LinearTween(time, p0.widthPixels, p1.widthPixels);
         smoothPoints.push_back(smPt);
      }
      // The first point is ALWAYS a beginning point.
      smoothPoints[0].position = LP_BEGIN;
      // If this is a REALLY short line, mark the end.
      if(p2.position == LP_END || p1.position == LP_END)
      {
         smoothPoints[smoothPoints.size()-1].position = LP_END;
      }
      // We need to mark the position where the second point begins in the
      // smoothed list.
      MarkSmoothingStart(1);
   }
   else
   {  // Beyond three points.
      // We can process the data
      ORIGINAL_POINT& p3 = orgPoints[newPointIndex];
      ORIGINAL_POINT& p2 = orgPoints[newPointIndex-1];
      ORIGINAL_POINT& p1 = orgPoints[newPointIndex-2];
      ORIGINAL_POINT& p0 = orgPoints[newPointIndex-3];
      
      MarkSmoothingStart(newPointIndex-2);
      
      float distPixels = ccpDistance(p2.point, p1.point);
      int ticks = MAX(4, distPixels/pixelsPerTick);
      double dt = 1.0/(ticks);
      SMOOTHED_POINT smPt;
      smPt.position = LP_CONTINUE;
      for(int idx = 0; idx < ticks; idx++)
      {
         float time = idx*dt;
         float tSq = time*time;
         float tCube = tSq*time;
         CCPoint pt;
         
         CCPoint b0 = ccpMult(p0.point,2*tSq -tCube - time);
         CCPoint b1 = ccpMult(p1.point,3*tCube-5*tSq + 2);
         CCPoint b2 = ccpMult(p2.point,4*tSq - 3*tCube + time);
         CCPoint b3 = ccpMult(p3.point,tCube-tSq);
         
         pt.x = 0.5*(b0.x+b1.x+b2.x+b3.x);
         pt.y = 0.5*(b0.y+b1.y+b2.y+b3.y);
         smPt.point = pt;
         smPt.widthPixels = MathUtilities::LinearTween(time, p1.widthPixels, p2.widthPixels);
         smoothPoints.push_back(smPt);
       }
      if(p3.position == LP_END)
      {  // Finish out the last section with a cubic hermite spline.
         p3.tangent = ccpSub(p3.point, p2.point);
         p2.tangent = ccpMult(ccpSub(p3.point, p1.point), 0.5f);
         
         // Mark the position for the smooth points index.
         MarkSmoothingStart(newPointIndex-1);
         
         // Fill in the curve between the first point and the second.
         float distPixels = ccpDistance(p3.point, p2.point);
         int ticks = MAX(4, distPixels/pixelsPerTick);
         double dt = 1.0/(ticks);
         SMOOTHED_POINT smPt;
         smPt.position = LP_CONTINUE;
         for(int idx = 0; idx <= ticks; idx++)
         {
            float time = idx*dt;
            smPt.point = HermiteSpline(time, p2.point, p3.point, p2.tangent, p3.tangent);
            smPt.widthPixels = MathUtilities::LinearTween(time, p2.widthPixels, p3.widthPixels);
            smoothPoints.push_back(smPt);
         }
         // The last point is the END point.
         smoothPoints[smoothPoints.size()-1].position = LP_END;
      }
   }
}