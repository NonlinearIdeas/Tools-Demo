/********************************************************************
 * File   : LineSmootherCardinal.h
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

#ifndef __ToolsDemo__LineSmootherCardinal__
#define __ToolsDemo__LineSmootherCardinal__

#include "LineSmoother.h"

class LineSmootherCardinal : public LineSmoother
{
protected:
   // Given a new original point at newPointIndex, calculate a new
   // set of smoothed points and add them to the smoothPoints list.
   virtual void CalculateSmoothPoints(uint32 newPointIndex);
private:
   float _tension;
public:
   void SetTension(float tension) { _tension = tension; }
   float GetTension() { return _tension; }
   
   LineSmootherCardinal();
};

#endif /* defined(__ToolsDemo__LineSmootherCardinal__) */
