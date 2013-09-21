/********************************************************************
 * File   : MainScene.h
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

#ifndef __Box2DTestBed__MainScene__
#define __Box2DTestBed__MainScene__

#include "CommonProject.h"
#include "CommonSTL.h"

#include "DebugLinesLayer.h"
#include "TapDragPinchInputLayer.h"
#include "Notifier.h"

class LineSmoother;

class MainScene : public CCScene, public Notified, public TapDragPinchInputLayerTarget
{
private:
   // This class follows the "create"/"autorelease" pattern.
   // Private constructor.
   MainScene();
   
   void ToggleShowingOriginal();
   
   uint32 _lastSmoothedIndex;
   LineSmoother* _lineSmoother;
   bool _showOriginal;
protected:
   // This is protected so that derived classes can call it
   // in their create methods.
   bool init();
   
private:
   void CreateMenu();
   void HandleMenuChoice(uint32 choice);
   void DrawOriginalLines();
   void DrawSmoothedLines();
   void DrawLines();
   void ResetDisplay();
public:
   
   static MainScene* create();
   
   ~MainScene();
   
   virtual void onEnter();
   virtual void onExit();
   virtual void onEnterTransitionDidFinish();
   virtual void onExitTransitionDidStart();
   
   // Handler for Notifier Events
   virtual void Notify(Notifier::NOTIFIED_EVENT_TYPE_T eventType, const void* eventData);
   
   // Handler for Tap/Drag/Pinch Events
   virtual void TapDragPinchInputTap(const CCPoint& point);
   virtual void TapDragPinchInputLongTap(const CCPoint& point);
   virtual void TapDragPinchInputPinchBegin(const CCPoint& point0, const CCPoint& point1);
   virtual void TapDragPinchInputPinchContinue(const CCPoint& point0, const CCPoint& point1);
   virtual void TapDragPinchInputPinchEnd(const CCPoint& point0, const CCPoint& point1);
   virtual void TapDragPinchInputDragBegin(const CCPoint& point0, const CCPoint& point1);
   virtual void TapDragPinchInputDragContinue(const CCPoint& point0, const CCPoint& point1);
   virtual void TapDragPinchInputDragEnd(const CCPoint& point0, const CCPoint& point1);
};


#endif /* defined(__Box2DTestBed__MainScene__) */
