/********************************************************************
 * File   : TapDragPinchInputLayer.h
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

#ifndef __Box2DTestBed__TapDragPinchInputLayer__
#define __Box2DTestBed__TapDragPinchInputLayer__

#include "CommonSTL.h"
#include "CommonProject.h"
#include "Stopwatch.h"

class TapDragPinchInputLayer;

/* This class is an interface for 
 * a delegate for the TapDragPinchInputLayer.
 * Any class (usually a scene) that wishes to 
 * receive the input from the layer derives
 * from this class and implements the virtual
 * methods in it.
 */

class TapDragPinchInputLayerTarget
{
private:
   /* These are used so that derived classes don't have to store the 
    * points from when a pinch began.
    */
   CCPoint _pinchPoint0;
   CCPoint _pinchPoint1;

   inline void SetPinchPoint0(const CCPoint& point0) { _pinchPoint0 = point0; }
   inline void SetPinchPoint1(const CCPoint& point1) { _pinchPoint1 = point1; }
   
   friend class TapDragPinchInputLayer;

protected:
   /* These points are stored automatically at the start of a pinch. 
    * This is because the pinch points can move over the course of the
    * pinch and this may or may not be signficant to the consumer
    * of the points.
    */
   const inline CCPoint& GetPinchPoint0() { return _pinchPoint0; }
   const inline CCPoint& GetPinchPoint1() { return _pinchPoint1; }
public:
   virtual void TapDragPinchInputTap(const CCPoint& point) = 0;
   virtual void TapDragPinchInputLongTap(const CCPoint& point) = 0;
   virtual void TapDragPinchInputPinchBegin(const CCPoint& point0, const CCPoint& point1) = 0;
   virtual void TapDragPinchInputPinchContinue(const CCPoint& point0, const CCPoint& point1) = 0;
   virtual void TapDragPinchInputPinchEnd(const CCPoint& point0, const CCPoint& point1) = 0;
   virtual void TapDragPinchInputDragBegin(const CCPoint& point0, const CCPoint& point1) = 0;
   virtual void TapDragPinchInputDragContinue(const CCPoint& point0, const CCPoint& point1) = 0;
   virtual void TapDragPinchInputDragEnd(const CCPoint& point0, const CCPoint& point1) = 0;
};

/*
 This class operates as a state machine.  A decision
 is made based on the order fingers go down, how far
 they have moved since they went down, etc., to decide
 if the input is a drag, a tap, or a pinch.  A tap
 is registered when the finger moves up.  A long
 tap is the same, with the amount of time since
 the "down" as the discriminator.
 
 
 The class sends out notifications via the Notifier.
 It also receives notifications to enable/disable input.
 
 */
class TapDragPinchInputLayer : public CCLayer, public CCTargetedTouchDelegate
{
   
private:
   
   //  The states it can be in.
   typedef enum
   {
      DPT_IDLE = 0,
      DPT_FINGER_DOWN,
      DPT_DRAG,
      DPT_PINCH,
      DPT_TAP,
   } DPT_STATE_T;
   
   typedef struct
   {
      CCPoint pos;
      int32 ID;
   } TOUCH_DATA_T;
   
   DPT_STATE_T _state;
   TOUCH_DATA_T _points[2];
   StopWatch _stopWatch;
   bool _enabled;
   
   bool init(TapDragPinchInputLayerTarget* target);
   TapDragPinchInputLayer();
   void StoreTouchData(CCTouch* touch, TOUCH_DATA_T* touchData);
   TapDragPinchInputLayerTarget* _target;
   
public:
   
   void DrawDebug();

   
   virtual ~TapDragPinchInputLayer();
   
   // The class registers/unregisters on entry
   // or exit of the layer.  This 
   virtual void onEnterTransitionDidFinish();
   virtual void onExitTransitionDidStart();
   
   virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
   virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
   virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
   virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
   
   
   static TapDragPinchInputLayer* create(TapDragPinchInputLayerTarget* target);
};

#endif /* defined(__Box2DTestBed__TapDragPinchInputLayer__) */
