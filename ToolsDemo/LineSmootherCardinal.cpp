/********************************************************************
 * File   : LineSmootherCardinal.cpp
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

#include "LineSmootherCardinal.h"
#include "MathUtilities.h"


LineSmootherCardinal::LineSmootherCardinal() :
   _tension(0.5)
{
   
}

void LineSmootherCardinal::CalculateSmoothPoints(uint32 newPointIndex)
{
   vector<ORIGINAL_POINT>& orgPoints = GetOriginalPoints();
   vector<SMOOTHED_POINT>& smoothPoints = GetSmoothedPoints();
   
   if(orgPoints.size() < 3)
   {  // Nothing to do with a single point.
      return;
   }
   ORIGINAL_POINT& p2 = orgPoints[newPointIndex];
   ORIGINAL_POINT& p1 = orgPoints[newPointIndex-1];
   ORIGINAL_POINT& p0 = orgPoints[newPointIndex-2];
   
   if(p0.position == LP_BEGIN)
   {  // Estimate the tangent for the start point of this new
      // line.
      p0.tangent.x = p1.point.x-p0.point.x;
      p0.tangent.y = p1.point.y-p0.point.y;
   }
   if(p2.position == LP_END)
   {
      p2.tangent.x = p2.point.x - p1.point.x;
      p2.tangent.y = p2.point.y - p1.point.y;
   }
   // Since the current point is not a start, it must be either the
   // end or a continue point.  We use the same rule for generating
   // the tangent in both cases.
   p1.tangent = ccpMult(ccpSub(p2.point, p0.point), _tension);

   
   // Now that we have tangents, estimate the spline between
   // the last point and the current point.
   // We want to have a relatively constant number of time ticks
   // based on the distance between the points.
   const float pixelsPerTick = 2.0;
   float distPixels = ccpDistance(p1.point, p0.point);
   int ticks = MAX(16, distPixels/pixelsPerTick);
   
   double dt = 1.0/(ticks);
   SMOOTHED_POINT smPt;
   for(int idx = 0; idx < ticks; idx++)
   {
      float time = idx*dt;
      if(p0.position == LP_BEGIN && idx == 0)
      {
         smPt.position = LP_BEGIN;
         smPt.point = p0.point;
      }
      else if(p1.position == LP_END && idx == (ticks))
      {
         smPt.position = LP_END;
         smPt.point = p1.point;
      }
      else
      {
         smPt.position = LP_CONTINUE;
         smPt.point = HermiteSpline(time, p0.point, p1.point, p0.tangent, p1.tangent);
      }
      smPt.widthPixels = MathUtilities::LinearTween(time, p0.widthPixels, p1.widthPixels);
      smoothPoints.push_back(smPt);
      //      CCLOG("Pushed point (%f,%f) onto smoothed points (size = %d) (position = %s)",
      //      smPt.point.x,smPt.point.y,smoothPoints.size(),smPt.position == LP_BEGIN?"BEGIN":smPt.position==LP_END?"END":"CONTINUE");
   }
   // Mark the index for the smoothed data.
   if(p2.position == LP_END)
   {
      // The last point is the END point.
      smoothPoints[smoothPoints.size()-1].position = LP_END;
   }
}