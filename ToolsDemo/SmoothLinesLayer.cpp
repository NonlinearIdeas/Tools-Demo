/********************************************************************
 * File   : SmoothLinesLayer.cpp
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


#include "SmoothLinesLayer.h"

//#define DEBUG_SMOOTH_LINES_LAYER


/* NOTE:  For debugging, it is occasionally handy to 
 * set the leve of overdraw to 10x (or more) of the 
 * baseline value so that you can *REALLY* see it 
 * well and make sure it is lining up with the ends.
 * Changing the draw color for overdraw also helps 
 * when things look a little off (so you can see
 * the triangles.
 */
const float OVERDRAW_LEVEL = 3.0;



SmoothLinesLayer::SmoothLinesLayer()
{
	
}

SmoothLinesLayer::~SmoothLinesLayer()
{
	
}

void SmoothLinesLayer::AddSmoothedPoints(const vector<LineSmoother::SMOOTHED_POINT>& smoothedPoints, uint32 startIdx)
{
   if(startIdx > 0)
      startIdx--;
   for(uint32 idx = startIdx; idx < smoothedPoints.size(); idx++)
   {
      _smoothedPoints.push_back(smoothedPoints[idx]);
   }

#ifdef DEBUG_SMOOTH_LINES_LAYER
   // Test Code
   _smoothedPoints.clear();
   SMOOTHED_POINT pt;
   
   pt.position = LineSmoother::LP_BEGIN;
   pt.point = ccp(200,384);
   pt.widthPixels = 20;
   _smoothedPoints.push_back(pt);
   
   pt.position = LineSmoother::LP_CONTINUE;
   pt.point = ccp(300,384);
   pt.widthPixels = 20;
   _smoothedPoints.push_back(pt);
   
   pt.position = LineSmoother::LP_CONTINUE;
   pt.point = ccp(400,484);
   pt.widthPixels = 20;
   _smoothedPoints.push_back(pt);
   
   pt.position = LineSmoother::LP_CONTINUE;
   pt.point = ccp(500,584);
   pt.widthPixels = 20;
   _smoothedPoints.push_back(pt);
   
   pt.position = LineSmoother::LP_CONTINUE;
   pt.point = ccp(600,584);
   pt.widthPixels = 20;
   _smoothedPoints.push_back(pt);
   
   pt.position = LineSmoother::LP_END;
   pt.point = ccp(650,584);
   pt.widthPixels = 20;
   _smoothedPoints.push_back(pt);
#endif
}


void SmoothLinesLayer::Reset()
{
   _smoothedPoints.clear();
   _renderTexture->clear(0.0, 0, 0, 0.0);
}

void SmoothLinesLayer::draw()
{
   CCLayer::draw();
   DrawSmoothLines();
}

SmoothLinesLayer* SmoothLinesLayer::create()
{
   SmoothLinesLayer *pRet = new SmoothLinesLayer();
   if (pRet && pRet->init())
   {
      pRet->autorelease();
      return pRet;
   }
   else
   {
      CC_SAFE_DELETE(pRet);
      return NULL;
   }
}


void SmoothLinesLayer::DrawVertices()
{
   glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), &_vertices[0].x);
   glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, sizeof(VERTEX), &_vertices[0].color);
   glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
   //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDrawArrays(GL_TRIANGLES, 0, _vertices.size());

   // Clear out, we are done with these.
   _vertices.clear();
}


void SmoothLinesLayer::DrawSmoothLines()
{
   // If there are no points to be processed,
   // don't bother.
   if(_smoothedPoints.size() < 3)
      return;
   
   // All drawing to the texture happens between the begin/end calls.
   _renderTexture->begin();
   
   // We can have "anything" in the list of smoothed points, so we have
   // to handle it that way.
   for(int idx = 2; idx < _smoothedPoints.size(); idx++)
   {
      const SMOOTHED_POINT& p0 = _smoothedPoints[idx-2];
      const SMOOTHED_POINT& p1 = _smoothedPoints[idx-1];
      const SMOOTHED_POINT& p2 = _smoothedPoints[idx-0];
      
      if(p0.position == LineSmoother::LP_BEGIN)
      {  // First point of a new line.
         DrawSmoothedLineSegment(p0, p2, p2);
         DrawHalfCircle(p1,p2,true);
      }
      DrawSmoothedLineSegment(p0, p1, p2);
      if(p2.position == LineSmoother::LP_END)
      {
         DrawSmoothedLineSegment(p1, p2, p2);
         DrawHalfCircle(p0,p1,false);
      }
   }
   
   // Do th actual drawing.
   DrawVertices();

   // Store off the last point.
   SMOOTHED_POINT lastPoint = _smoothedPoints[_smoothedPoints.size()-1];
   // We're done, clear out the memory.
   _smoothedPoints.clear();
   
   // End drawing to the texture.
   _renderTexture->end();
   
}


void SmoothLinesLayer::DrawHalfCircle(const SMOOTHED_POINT& p0, const SMOOTHED_POINT& p1, bool flip)
{
#ifdef DEBUG_SMOOTH_LINES_LAYER
   CCLOG("Half Circle: (%f,%f) -> (%f,%f)",
         p0.point.x,p0.point.y,
         p1.point.x,p1.point.y);
#endif
   ccColor4F drawColorClear =  ccc4f(_drawColor.r, _drawColor.g, _drawColor.b, 0.0f);
   ccColor4F drawColor = _drawColor;
   const int slices = 16;
   float dtheta = M_PI/slices;
   VERTEX vertex;
   
   CCPoint p0p = p0.point;
   CCPoint p1p = p1.point;
   if(flip)
   {  // Flip the half circle so it faces the other direction.  The circle
      // is flipped along the axis from p0 to p1.
      p1p = ccpAdd(p0p,ccpMult(ccpSub(p0p, p1p), 2));
   }
   float p0width = p0.widthPixels;
   
   CCPoint p01np = ccpPerp(ccpNormalize(ccpSub(p0p, p1p)));
   float thetaStart = atan2f(p01np.y, p01np.x);
   
   float cosCurr = cosf(thetaStart);
   float sinCurr = sinf(thetaStart);
   
   for(int idx = 1; idx < slices; idx++)
   {
      float theta = thetaStart + dtheta*idx;
      
      // Try to only do this computation once.  The vector created from ccp(cos(theta),sin(theta))
      // is used again the next time through as the previous slice side.
      float cosNext = cosf(theta+dtheta);
      float sinNext = sinf(theta+dtheta);
      
      CCPoint A = ccpAdd(p0p, ccpMult(ccp(cosCurr,sinCurr),p0width/2));
      CCPoint B = ccpAdd(p0p, ccpMult(ccp(cosNext,sinNext),p0width/2));
      CCPoint C = ccpAdd(p0p, ccpMult(ccp(cosCurr,sinCurr),p0width/2+OVERDRAW_LEVEL));
      CCPoint D = ccpAdd(p0p, ccpMult(ccp(cosNext,sinNext),p0width/2+OVERDRAW_LEVEL));
      
      cosCurr = cosNext;
      sinCurr = sinNext;
      
      // Main Triangle (1)
      vertex.x = p0p.x;vertex.y = p0p.y; vertex.z = 1.0; vertex.color = drawColor; _vertices.push_back(vertex);
      vertex.x = A.x;vertex.y = A.y; vertex.z = 1.0; vertex.color = drawColor; _vertices.push_back(vertex);
      vertex.x = B.x;vertex.y = B.y; vertex.z = 1.0; vertex.color = drawColor; _vertices.push_back(vertex);
      
      // Overdraw triangles (2)
      // ACD
      vertex.x = A.x;vertex.y = A.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
      vertex.x = C.x;vertex.y = C.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
      vertex.x = D.x;vertex.y = D.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
      // ADB
      vertex.x = A.x;vertex.y = A.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
      vertex.x = D.x;vertex.y = D.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
      vertex.x = B.x;vertex.y = B.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
      
   }
}


void SmoothLinesLayer::DrawSmoothedLineSegment(const SMOOTHED_POINT& p0, const SMOOTHED_POINT& p1, const SMOOTHED_POINT& p2)
{
#ifdef DEBUG_SMOOTH_LINES_LAYER
   CCLOG("Draw Smoothed Line: (%f,%f) -> (%f,%f)",
         p0.point.x,p0.point.y,
         p1.point.x,p1.point.y);
#endif
   VERTEX vertex;
   ccColor4F drawColorClear =  ccc4f(_drawColor.r, _drawColor.g, _drawColor.b, 0.0f);
   
   CCPoint p0p = p0.point;
   CCPoint p1p = p1.point;
   CCPoint p2p = p2.point;
   float p0width = p0.widthPixels;
   float p1width = p1.widthPixels;
   

   CCPoint p01n = ccpNormalize(ccpSub(p1p, p0p));
   CCPoint p12n = ccpNormalize(ccpSub(p2p, p1p));
   CCPoint p01np = ccpPerp(p01n);
   CCPoint p12np = ccpPerp(p12n);
   
   CCPoint A = ccpAdd(p0p, ccpMult(p01np, p0width/2));
   CCPoint B = ccpAdd(p1p, ccpMult(p12np, p1width/2));
   CCPoint C = ccpSub(p0p, ccpMult(p01np, p0width/2));
   CCPoint D = ccpSub(p1p, ccpMult(p12np, p1width/2));
   
   CCPoint E = ccpAdd(p0p, ccpMult(p01np, OVERDRAW_LEVEL + p0width/2));
   CCPoint F = ccpAdd(p1p, ccpMult(p12np, OVERDRAW_LEVEL + p1width/2));
   CCPoint G = ccpSub(p0p, ccpMult(p01np, OVERDRAW_LEVEL + p0width/2));
   CCPoint H = ccpSub(p1p, ccpMult(p12np, OVERDRAW_LEVEL + p1width/2));

   // Do the main line segment triangles (2)
   // ABC
   vertex.x = A.x;vertex.y = A.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = B.x;vertex.y = B.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = C.x;vertex.y = C.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   
   // CBD
   vertex.x = C.x;vertex.y = C.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = B.x;vertex.y = B.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = D.x;vertex.y = D.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   
   // Do the overdraw triangles (4)
   // Note that the vertices on the external edges have clear color (alpha = 0.0f).
   // AEF
   vertex.x = A.x;vertex.y = A.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = E.x;vertex.y = E.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
   vertex.x = F.x;vertex.y = F.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
   
   // AFB
   vertex.x = A.x;vertex.y = A.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = F.x;vertex.y = F.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
   vertex.x = B.x;vertex.y = B.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);

   // CDG
   vertex.x = C.x;vertex.y = C.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = D.x;vertex.y = D.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = G.x;vertex.y = G.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
   
   // GDH
   vertex.x = G.x;vertex.y = G.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
   vertex.x = D.x;vertex.y = D.y; vertex.z = 1.0; vertex.color = _drawColor; _vertices.push_back(vertex);
   vertex.x = H.x;vertex.y = H.y; vertex.z = 1.0; vertex.color = drawColorClear; _vertices.push_back(vertex);
}


bool SmoothLinesLayer::init()
{
   if(!CCLayer::init())
      return false;
   
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   
   _renderTexture = CCRenderTexture::create(scrSize.width, scrSize.height, kCCTexture2DPixelFormat_RGBA8888);
   _renderTexture->setPosition(ccp(scrSize.width/2,scrSize.height/2));
   addChild(_renderTexture);
   Reset();
   
   _smoothedPoints.reserve(500);

   // Set this as the shader program for this layer.  This is one of the default shaders
   // in cocos2d-x (and cocos2d). 
   setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionColor));
   
   return true;
}