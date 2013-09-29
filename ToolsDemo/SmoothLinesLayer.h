/********************************************************************
 * File   : SmoothLinesLayer.h
 * Project: ToolsDemo
 *
 ********************************************************************
 * Created on 9/24/13 By Nonlinear Ideas Inc.
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

#ifndef __ToolsDemo__SmoothLinesLayer__
#define __ToolsDemo__SmoothLinesLayer__

#include "CommonSTL.h"
#include "CommonProject.h"
#include "LineSmoother.h"

class SmoothLinesLayer : public CCLayer
{
private:
   typedef LineSmoother::SMOOTHED_POINT SMOOTHED_POINT;
   typedef struct {
      float x;
      float y;
      float z;
      ccColor4F color;
   } VERTEX;
   
   
   ccColor4F _drawColor;
   vector<VERTEX> _vertices;
   vector<SMOOTHED_POINT> _smoothedPoints;
   CCRenderTexture* _renderTexture;
      
   bool init();
   
   void DrawSmoothLines();
   void DrawHalfCircle(const SMOOTHED_POINT& p0, const SMOOTHED_POINT& p1, bool flip, float width);
   void DrawSmoothedLineSegment(const SMOOTHED_POINT& p0, const SMOOTHED_POINT& p1, const SMOOTHED_POINT& p2);
   void DrawVertices();
   
   
   
   SmoothLinesLayer();
   ~SmoothLinesLayer();

public:
   
   void Reset();
   void AddSmoothedPoints(const vector<LineSmoother::SMOOTHED_POINT>& smoothedPoints, uint32 startIdx);
   void SetDrawColor(const ccColor4F& drawColor) { _drawColor = drawColor; }
   const ccColor4F& GetDrawColor() { return _drawColor; }
   
   virtual void draw();
   static SmoothLinesLayer* create();
};

#endif /* defined(__ToolsDemo__SmoothLinesLayer__) */
