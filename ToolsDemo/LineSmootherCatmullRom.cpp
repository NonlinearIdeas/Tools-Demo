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
      ORIGINAL_POINT& p0 = orgPoints[newPointIndex-2];
      ORIGINAL_POINT& p1 = orgPoints[newPointIndex-1];
      ORIGINAL_POINT& p2 = orgPoints[newPointIndex];
      
      // Fill in the curve between the first point and the second.
      float distPixels = ccpDistance(p0.point, p1.point);
      int ticks = MAX(4, distPixels/pixelsPerTick);
      double dt = 1.0/(ticks);
      SMOOTHED_POINT smPt;
      smPt.position = LP_CONTINUE;
      for(int idx = 0; idx < ticks; idx++)
      {
         float time = idx*dt;
         smPt.point.x = MathUtilities::LinearTween(time,p0.point.x,p1.point.x);
         smPt.point.y = MathUtilities::LinearTween(time,p0.point.y,p1.point.y);
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
   }
   else
   {  // Beyond three points.
      // We can process the data
      ORIGINAL_POINT& p3 = orgPoints[newPointIndex];
      ORIGINAL_POINT& p2 = orgPoints[newPointIndex-1];
      ORIGINAL_POINT& p1 = orgPoints[newPointIndex-2];
      ORIGINAL_POINT& p0 = orgPoints[newPointIndex-3];
      
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
      {
         // The last point is the END point.
         smoothPoints[smoothPoints.size()-1].position = LP_END;
      }
   }
}