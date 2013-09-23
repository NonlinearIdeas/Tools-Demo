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
      float32 widthPixels;
      
      void Init(const CCPoint& point_,
                LINE_POSITION_T position_)
      {
         point = point_;
         position = position;
         widthPixels = 1.0;
      }
      
      void Dump(int pointIdx = -1) const
      {
         CCLOG("SMOOTH POINT(%d): POINT:(%f,%f), POS:(%s), WIDTH:(%f)",
               pointIdx,
               point.x,point.y,
               position == LP_BEGIN?"BEGIN":position == LP_CONTINUE?"CONTINUE":"END",
               widthPixels
               );
      }
   };
   
   struct ORIGINAL_POINT
   {
      CCPoint point;
      double timestamp;
      LINE_POSITION_T position;
      CCPoint tangent;
      uint32 smoothedStartIndex;
      float pointsPerSecond;
      float widthPixels;
      
      void Init(const CCPoint& point_,
           double timestamp_,
           const LINE_POSITION_T& position_,
           const CCPoint& tangent_)
      {
         point = point_;
         position = position_;
         tangent = tangent_;
         timestamp = timestamp_;
         smoothedStartIndex = 0;
         pointsPerSecond = 1.0f;
         widthPixels = 1.0f;
      }
      void Dump(int pointIdx=-1) const
      {
         CCLOG("ORIGINAL POINT(%d): POINT:(%f,%f), POS:(%s), TAN:(%f,%f), TIME:(%f), SSI:(%u), WIDTH:(%f), VEL:(%f)",
               pointIdx,
               point.x,point.y,
               position == LP_BEGIN?"BEGIN":position == LP_CONTINUE?"CONTINUE":"END",
               tangent.x,tangent.y,
               timestamp,
               smoothedStartIndex,
               widthPixels,
               pointsPerSecond
               );
      }
   };
   
private:
   // ALL the point for this line accumulated.
   vector<ORIGINAL_POINT> _orgPoints;
   
   // The smoothed points created for this line.
   vector<SMOOTHED_POINT> _smoothPoints;
   
   // Keeps the index of the last smoothed index point
   // retrieved for a client so they can pick up where
   // they left off when  drawing a single group at a time.
   uint32 _lastSmoothPointIndex;
   
   // Called each time a new point original is added.
   // This method is overriden in derived classes to add
   // new types of spline options.
   void ProcessNewPoint();
   
protected:

   // The following functions are called internally by the base class as
   // an algorithm to create the smoothed lines.  The functions are called
   // in the following order:
   //
   // CalculateVelocities(...)
   // CalculateWidths(...)
   // CalculateSmoothPoints(...)
   //
   // The idea is that as new original points come in, some calculation
   // of the velocities will be done and these values deposited in the
   // information for the new original point.  The the widths will be
   // calculated based on the velocities and position relative to the
   // start of the set.  Finally the smooth points themselves will be
   // calculate.
   //
   //
   
   // Given a new original point at newPointIndex, calculate a new
   // set of smoothed points and add them to the smoothPoints list.
   virtual void CalculateSmoothPoints(uint32 newPointIndex);
   // Estimate the velocity of the new point relative to previous points
   // in pixels/second.
   virtual void CalculateVelocities(uint32 newPointIndex);
   // Calculate velocities for the original new point at newPointIndex.
   virtual void CalculateWidths(uint32 newPointIndex);
   
   // Methods used by derived classes to implement ProcessNewPoint
   vector<ORIGINAL_POINT>& GetOriginalPoints() { return _orgPoints; }
   vector<SMOOTHED_POINT>& GetSmoothedPoints() { return _smoothPoints; }
   void AddSmoothedPoint(const SMOOTHED_POINT& sm) { _smoothPoints.push_back(sm); }
   CCPoint HermiteSpline(float t, CCPoint p0, CCPoint p1, CCPoint m0, CCPoint m1);
   
   // Call this from your smoothing algorithm to mark the size of the smoothPoints
   // list into the original point set.  You don't have to do this if you don't
   // use the smoothedStartIndex later.
   void MarkSmoothingStart(uint32 pointIndex);
   
public:
   // Resets the data points.
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
   const vector<ORIGINAL_POINT>& GetOriginalPointsConst() const { return _orgPoints; }
   // New smoothed points are added to this array.  You can use the points in the original
   // data to line up the positions of the points in this array.  When a new line is started
   // both arrays are cleared.
   const vector<SMOOTHED_POINT>& GetSmoothedPointsConst() const { return _smoothPoints; }
   void MarkLastSmoothPointIndex() { _lastSmoothPointIndex = _smoothPoints.size(); }
   uint32 GetLastSmoothPointIndex() { return _lastSmoothPointIndex; }
};

#endif /* defined(__ToolsDemo__LineSmoother__) */
