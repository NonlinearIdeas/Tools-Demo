/********************************************************************
 * File   : LineSmoother.h
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

#ifndef __ToolsDemo__LineSmoother__
#define __ToolsDemo__LineSmoother__

#include "CommonProject.h"
#include "CommonSTL.h"

class LineSmoother
{
public:
   
   typedef enum
   {
      LP_BEGIN = 0,
      LP_CONTINUE,
      LP_END,
      LP_MAX
   } LINE_POSITION_T;
      
   struct SMOOTHED_POINT
   {
      CCPoint point;
      LINE_POSITION_T position;
      
      void Init(const CCPoint& point_,
                LINE_POSITION_T position_)
      {
         point = point_;
         position = position;
      }
   };
   
   struct ORIGINAL_POINT
   {
      CCPoint point;
      double timestamp;
      LINE_POSITION_T position;
      CCPoint tangent;
      uint32 smoothedStartIndex;
      
      void Init(const CCPoint& point_,
           double timestamp_,
           const LINE_POSITION_T& position_,
           const CCPoint& tangent_,
                uint32 smoothedStartIndex_)
      {
         point = point_;
         position = position_;
         tangent = tangent_;
         timestamp = timestamp_;
         smoothedStartIndex = smoothedStartIndex_;
      }
   };
   
private:
   // ALL the points accumulated.
   vector<ORIGINAL_POINT> _orgPoints;
   // The smoothed points created from the last point added
   // to the data set.  
   vector<SMOOTHED_POINT> _smoothPoints;
protected:
   // Called each time a new point original is added.
   // This method is overriden in derived classes to add
   // new types of spline options.
   virtual void ProcessNewPoint();
   // Methods used by derived classes to implement ProcessNewPoint
   vector<ORIGINAL_POINT>& GetOriginalPointsInternal() { return _orgPoints; }
   vector<SMOOTHED_POINT>& GetSmoothedPointsInternal() { return _smoothPoints; }
   void AddSmoothedPoint(const SMOOTHED_POINT& sm) { _smoothPoints.push_back(sm); }
   CCPoint HermiteSpline(float t, CCPoint p0, CCPoint p1, CCPoint m0, CCPoint m1);
   
public:
   // Resets the data points, but does not change the
   // algorithm used.
   void Reset();
   
   // Add points to the line using the following calls.  It is expected that
   // the caller will call them in the "right" order (i.e. first point, some other points, last point).
   // If not, this class will throw an exception.
   void LineBegin(const CCPoint& point0, double timestamp);
   void LineContinue(const CCPoint& point, double timestamp);
   void LineEnd(const CCPoint& point, double timestamp);
   
   LineSmoother();
   virtual ~LineSmoother();
   

public:
   // Use this to retrieve the original set of points.  This can be useful for debugging
   // or if you wish to redraw the entire set (i.e. copy, reset, and resubmit them).
   const vector<ORIGINAL_POINT>& GetOriginalPoints() { return _orgPoints; }
   // New smoothed points are added to this array.  You can use the points in the original
   // data to line up the positions of the points in this array.  When a new line is started
   // both arrays are cleared.
   const vector<SMOOTHED_POINT>& GetSmoothedPoints() { return _smoothPoints; }
};

#endif /* defined(__ToolsDemo__LineSmoother__) */
