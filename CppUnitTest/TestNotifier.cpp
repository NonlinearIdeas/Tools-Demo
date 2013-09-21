/********************************************************************
 * File   : TestNotifier.cpp
 * Project: CppUnitTest
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

#include "TestNotifier.h"
#include "Notifier.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TestNotifier);

/* We're going to need a class to receive notifications and give us some 
 * info about it when needed.
 */

class NotifiedTarget : public Notified
{
private:
   string _name;
   
   typedef struct
   {
      Notifier::NOTIFIED_EVENT_TYPE_T eventType;
      const void* eventData;
   } NOTIFY_PAIR_T;

   vector<NOTIFY_PAIR_T> _notifications;
   
   NotifiedTarget() {}
public:
   const vector<NOTIFY_PAIR_T>& GetNotifications() { return _notifications; }
   const string& GetName() { return _name; }
   
   void Reset() { _notifications.clear(); }
   
   virtual void Notify(Notifier::NOTIFIED_EVENT_TYPE_T eventType, const void* eventData)
   {
      NOTIFY_PAIR_T pair;
      pair.eventType = eventType;
      pair.eventData = eventData;
      _notifications.push_back(pair);
   }
   
   NotifiedTarget(string name) : _name(name)
   {
      
   }
};

TestNotifier::TestNotifier()
{
   
}

TestNotifier::~TestNotifier()
{
   
}

void TestNotifier::setUp()
{
   CPPUNIT_ASSERT(Notifier::Instance().Init() == false);
}

void TestNotifier::tearDown()
{
   Notifier::Instance().Shutdown();
}

void TestNotifier::TestInit()
{
}