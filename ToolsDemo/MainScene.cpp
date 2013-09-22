/********************************************************************
 * File   : MainScene.cpp
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

#include "MainScene.h"
#include "LineSmootherCatmullRom.h"
#include "LineSmootherCardinal.h"
#include "DebugLinesLayer.h"
#include "DebugMenuLayer.h"
#include "TapDragPinchInputLayer.h"



MainScene::MainScene()
{
   _lineSmoother = new LineSmootherCatmullRom();
   //   _lineSmoother = new LineSmootherCardinal();
}

MainScene::~MainScene()
{
   delete _lineSmoother;
}

bool MainScene::init()
{
   
   CCLayer* layer;
   
   // Add a color background.  This will make it easier on the eyes.
   layer = CCLayerColor::create(ccc4(240, 240, 240, 255));
   assert(layer!=NULL);
   addChild(layer);
   
   
   // Adding the debug lines so that we can draw the original
   // and smoothed data.
   layer = DebugLinesLayer::create();
   assert(layer != NULL);
   addChild(layer);
   
   // Add the menu.
   CreateMenu();
   
   // Touch Input Layer
   layer = TapDragPinchInputLayer::create(this);
   assert(layer != NULL);
   addChild(layer);
   
   _showOriginal = false;
   
   return true;
}

MainScene* MainScene::create()
{
   MainScene *pRet = new MainScene();
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

void MainScene::onEnter()
{
   CCScene::onEnter();
}

void MainScene::onExit()
{
   CCScene::onExit();
}

void MainScene::onEnterTransitionDidFinish()
{
   CCScene::onEnterTransitionDidFinish();
   // It is a good practice to attach/detach from the Notifier
   // on screen transition times.  This gets you out of the question
   // of when the scene deletion occurs.
   Notifier::Instance().Attach(this, Notifier::NE_DEBUG_BUTTON_PRESSED);
}

void MainScene::onExitTransitionDidStart()
{
   CCScene::onExitTransitionDidStart();
   // It is a good practice to attach/detach from the Notifier
   // on screen transition times.  This gets you out of the question
   // of when the scene deletion occurs.
   Notifier::Instance().Detach(this);
}

// Handler for Notifier Events
void MainScene::Notify(Notifier::NOTIFIED_EVENT_TYPE_T eventType, const void* eventData)
{
   switch(eventType)
   {
      default:
         assert(false);
         break;
      case Notifier::NE_DEBUG_BUTTON_PRESSED:
         HandleMenuChoice((uint32)eventData);
         break;
   }
}

// Handler for Tap/Drag/Pinch Events
void MainScene::TapDragPinchInputTap(const TOUCH_DATA_T& point)
{
   
}
void MainScene::TapDragPinchInputLongTap(const TOUCH_DATA_T& point)
{
   ResetDisplay();
}
void MainScene::TapDragPinchInputPinchBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   
}
void MainScene::TapDragPinchInputPinchContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   
}
void MainScene::TapDragPinchInputPinchEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   
}
void MainScene::TapDragPinchInputDragBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   _lineSmoother->LineBegin(point0.pos,point0.timestamp);
   _lineSmoother->LineContinue(point1.pos,point1.timestamp);
   DrawLines();
}
void MainScene::TapDragPinchInputDragContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   _lineSmoother->LineContinue(point1.pos,point1.timestamp);
   DrawLines();
}
void MainScene::TapDragPinchInputDragEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   _lineSmoother->LineEnd(point1.pos,point1.timestamp);
   DrawLines();
}

void MainScene::CreateMenu()
{
   vector<string> labels;
   labels.push_back("Reset Lines");
   labels.push_back("Toggle Original");
   DebugMenuLayer* layer = DebugMenuLayer::create(labels);
   layer->GetMenu()->setColor(ccc3(0, 0, 0));
   assert(layer != NULL);
   addChild(layer);
}

void MainScene::ResetDisplay()
{
   _lineSmoother->Reset();
   Notifier::Instance().Notify(Notifier::NE_RESET_DRAW_CYCLE);
}

void MainScene::ToggleShowingOriginal()
{
   _showOriginal = !_showOriginal;
   ResetDisplay();
}

void MainScene::HandleMenuChoice(uint32 choice)
{
   switch(choice)
   {
      case 0:
         ResetDisplay();
         break;
      case 1:
         ToggleShowingOriginal();
         break;
      default:
         assert(false);
         break;
   }
}

void MainScene::DrawLines()
{
   if(_showOriginal)
   {
      DrawOriginalLines();
   }
   DrawSmoothedLines();
}

static void DumpPointData(const LineSmoother::ORIGINAL_POINT& point)
{
   CCLOG("Drawing Data for original point: (%f,%f), position = %s, timestamp = %f",
         point.point.x,point.point.y,
         point.position == LineSmoother::LP_BEGIN?"BEGIN":point.position == LineSmoother::LP_CONTINUE?
         "CONTINUE":"END",point.timestamp);
}

void MainScene::DrawOriginalLines()
{
   LINE_PIXELS_DATA lp;
   ccColor4F lineColor = ccc4f(0.8, 0.1, 0.1, 0.75);
   const vector<LineSmoother::ORIGINAL_POINT>& points = _lineSmoother->GetOriginalPoints();
   // Clear ALL lines out of the debug drawing.
   
   // Get the original points, draw them
   lp.color = lineColor;
   if(points.size() > 1)
   {
      int idx = points.size()-1;
      const LineSmoother::ORIGINAL_POINT& point = points[idx-1];
      const LineSmoother::ORIGINAL_POINT& nextPoint = points[idx];
      
      // Cases for lines:
      // 1. We are at the start of the line.
      // 2. We are at the end of a line.
      // 3. We are in the middle of a line.
      
      // Dumping data for timestamps for the original lines
      // This is part of the process of finding the velocity of the
      // user's pixels/second so we can do "interesting things" with
      // it.
      if(points.size() == 2)
      {
         DumpPointData(point);
      }
      DumpPointData(nextPoint);
      
      if(point.position == LineSmoother::LP_BEGIN)
      {  // Start of the line.
         lp.markerRadius = 8.0;
         lp.start = point.point;
         lp.end = point.point;
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
         lp.start = point.point;
         lp.end = nextPoint.point;
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
      }
      else if(point.position == LineSmoother::LP_END)
      {  // End of the line.
         lp.start = point.point;
         lp.end = point.point;
         lp.markerRadius = 12.0;
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
      }
      else
      {  // All the other points are between begin/end.
         lp.markerRadius = 2.0;
         lp.start = point.point;
         lp.end = nextPoint.point;
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
      }
      // We still have to draw the "last" last point, Since we get an update every
      // time a new point is added, we have to check if the last point is an end point
      // or not.  If it is, give it a special marker.  This will be the "last point of
      // the last line."
      const LineSmoother::ORIGINAL_POINT lastPoint = points[points.size()-1];
      if(lastPoint.position == LineSmoother::LP_END)
      {
         lp.start = lastPoint.point;
         lp.end = lastPoint.point;
         lp.markerRadius = 24.0;
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
      }
      
   }
}

void MainScene::DrawSmoothedLines()
{
   LINE_PIXELS_DATA lp;
   ccColor4F lineColor = ccc4f(0.15, 0.8, 0.1, 0.95);
   const vector<LineSmoother::SMOOTHED_POINT>& points = _lineSmoother->GetSmoothedPoints();
   // Clear ALL lines out of the debug drawing.
   
   // Get the original points, draw them
   lp.color = lineColor;
   if(points.size() > 1)
   {
      for(int idx = _lineSmoother->GetLastSmoothPointIndex(); idx < points.size(); idx++)
      {
         const LineSmoother::SMOOTHED_POINT& point = points[idx-1];
         const LineSmoother::SMOOTHED_POINT& nextPoint = points[idx];
         
         // Cases for lines:
         // 1. We are at the start of the line.
         // 2. We are at the end of a line.
         // 3. We are in the middle of a line.
         
         if(point.position == LineSmoother::LP_BEGIN)
         {  // Start of the line.
            lp.markerRadius = 0.0;
            lp.start = point.point;
            lp.end = point.point;
            Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
            lp.start = point.point;
            lp.end = nextPoint.point;
            Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
            //           CCLOG("Drawing BEGIN: (%f,%f)",point.point.x,point.point.y);
         }
         else if(point.position == LineSmoother::LP_END)
         {  // End of the line.
            lp.start = point.point;
            lp.end = point.point;
            lp.markerRadius = 0.0;
            Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
            /*
            CCLOG("Drawing END (%d of %u): (%f,%f)",
                  idx,points.size(),
                  point.point.x,point.point.y);
             */
         }
         else
         {  // All the other points are between begin/end.
            lp.markerRadius = 0.0;
            lp.start = point.point;
            lp.end = nextPoint.point;
            Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
            /*
             CCLOG("Drawing CONTINUE (%d of %u): (%f,%f) -> (%f,%f)",
                  idx,points.size(),
                  point.point.x,point.point.y,
                  nextPoint.point.x,nextPoint.point.y
                  );
             */
         }
      }
      // Mark the last smooth point set retrieved so that we can pick up here on the next
      // point set.
      _lineSmoother->MarkLastSmoothPointIndex();
      // We still have to draw the "last" last point, Since we get an update every
      // time a new point is added, we have to check if the last point is an end point
      // or not.  If it is, give it a special marker.  This will be the "last point of
      // the last line."
      const LineSmoother::SMOOTHED_POINT lastPoint = points[points.size()-1];
      if(lastPoint.position == LineSmoother::LP_END)
      {
         lp.start = lastPoint.point;
         lp.end = lastPoint.point;
         lp.markerRadius = 0.0;
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,&lp);
         //      CCLOG("Drawing END: (%f,%f)",lastPoint.point.x,lastPoint.point.y);
      }
   }   
}

