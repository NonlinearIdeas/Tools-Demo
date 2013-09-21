/********************************************************************
 * File   : AppDelegate.cpp
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


#include "AppDelegate.h"

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "MainScene.h"
#include "Notifier.h"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate()
{
   
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
   // initialize director
   CCDirector *pDirector = CCDirector::sharedDirector();
   pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
   
   // We need to do this so that the displayed items will all be at the
   // same pixel positions.
   CCEGLView::sharedOpenGLView()->setDesignResolutionSize(1024, 768, kResolutionNoBorder);
   
   // turn on display FPS
   pDirector->setDisplayStats(true);
   
   // set FPS. the default value is 1.0/60 if you don't call this
   pDirector->setAnimationInterval(1.0 / 60);
   
   Notifier::Instance().Init();
   
   // create a scene. it's an autorelease object
   CCScene *pScene = MainScene::create();
   
   // run
   pDirector->runWithScene(pScene);
   
   return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
   CCDirector::sharedDirector()->stopAnimation();
   SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
   SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
   CCDirector::sharedDirector()->startAnimation();
   SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
   SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}
