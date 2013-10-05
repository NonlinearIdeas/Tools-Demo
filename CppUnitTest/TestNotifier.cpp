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

typedef void (*FUNCTION)(void);

class NotifyTarget : public Notified
{
public:
   typedef struct
   {
      Notifier::NOTIFIED_EVENT_TYPE_T eventType;
      const void* eventData;
   } NOTIFY_PAIR_T;
   
private:
   string _name;
   vector<NOTIFY_PAIR_T> _notifications;
   
   NotifyTarget() {}
public:
   vector<NOTIFY_PAIR_T> GetNotifications() { vector<NOTIFY_PAIR_T> result = _notifications; return result; }
   const string& GetName() { return _name; }
   
   void Reset() { _notifications.clear(); }
   
   virtual void Notify(Notifier::NOTIFIED_EVENT_TYPE_T eventType, const void* eventData)
   {
      NOTIFY_PAIR_T pair;
      pair.eventType = eventType;
      pair.eventData = eventData;
      _notifications.push_back(pair);
      
      if(eventData != NULL)
      {  // Treat it as a FUNCTION
         FUNCTION f = (FUNCTION)eventData;
         f();
      }
   }
   
   NotifyTarget(string name) : _name(name)
   {
      
   }
};

NotifyTarget* pNotifyTarget1;
NotifyTarget* pNotifyTarget2;
NotifyTarget* pNotifyTarget3;

// Some static helper functions.
static void RegisterTestObjects()
{
   Notifier::Instance().Attach(pNotifyTarget1, Notifier::NE_DEBUG_BUTTON_PRESSED);
   Notifier::Instance().Attach(pNotifyTarget1, Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS);
   Notifier::Instance().Attach(pNotifyTarget2, Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY);
   Notifier::Instance().Attach(pNotifyTarget2, Notifier::NE_RESET_DRAW_CYCLE);
   Notifier::Instance().Attach(pNotifyTarget3, Notifier::NE_DEBUG_BUTTON_PRESSED);
   Notifier::Instance().Attach(pNotifyTarget3, Notifier::NE_RESET_DRAW_CYCLE);
}

static void DeleteNotifyTarget(NotifyTarget* pNotifyTarget)
{
   if(pNotifyTarget == NULL)
      return;
   
   if(pNotifyTarget == pNotifyTarget1)
   {
      delete pNotifyTarget1;
      pNotifyTarget1 = NULL;
   }
   if(pNotifyTarget == pNotifyTarget2)
   {
      delete pNotifyTarget2;
      pNotifyTarget2 = NULL;
   }
   if(pNotifyTarget == pNotifyTarget3)
   {
      delete pNotifyTarget3;
      pNotifyTarget3 = NULL;
   }
}


// We use these later when we have to have a
// scenario where a notification deletes another object
// that may be called as part of a notification.
static void DeleteNotifyTarget2()
{
   DeleteNotifyTarget(pNotifyTarget2);
}

static void DeleteNotifyTarget3()
{
   DeleteNotifyTarget(pNotifyTarget3);
}

static void ResetNotifyTargets()
{
   if(pNotifyTarget1 != NULL)
      pNotifyTarget1->Reset();
   if(pNotifyTarget2 != NULL)
      pNotifyTarget2->Reset();
   if(pNotifyTarget3 != NULL)
      pNotifyTarget3->Reset();
}


TestNotifier::TestNotifier()
{
   
}

TestNotifier::~TestNotifier()
{
   
}


void TestNotifier::setUp()
{
   CPPUNIT_ASSERT(Notifier::Instance().Init() == true);
   pNotifyTarget1 = new NotifyTarget("Notify Target 1");
   pNotifyTarget2 = new NotifyTarget("Notify Target 2");
   pNotifyTarget3 = new NotifyTarget("Notify Target 3");
}

void TestNotifier::tearDown()
{
   DeleteNotifyTarget(pNotifyTarget1);
   DeleteNotifyTarget(pNotifyTarget2);
   DeleteNotifyTarget(pNotifyTarget3);
   CPPUNIT_ASSERT_NO_THROW(Notifier::Instance().Shutdown());
}



// Verify we can fetch all the messages for an object.
void TestNotifier::TestFetchMessagesForObject()
{
   vector<Notifier::NOTIFIED_EVENT_TYPE_T> events;
   
   // First, veriy that these objects are registered for nothing.
   events = Notifier::Instance().GetEvents(pNotifyTarget1);
   CPPUNIT_ASSERT(events.size() == 0);
   events = Notifier::Instance().GetEvents(pNotifyTarget2);
   CPPUNIT_ASSERT(events.size() == 0);
   events = Notifier::Instance().GetEvents(pNotifyTarget3);
   CPPUNIT_ASSERT(events.size() == 0);
   
   // Add some events
   RegisterTestObjects();
   
   // Verify the correct messages are there for each object.
   events = Notifier::Instance().GetEvents(pNotifyTarget1);
   CPPUNIT_ASSERT(events.size() == 2);
   CPPUNIT_ASSERT(events[0] == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(events[1] == Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS);
   events = Notifier::Instance().GetEvents(pNotifyTarget2);
   CPPUNIT_ASSERT(events.size() == 2);
   CPPUNIT_ASSERT(events[0] == Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY);
   CPPUNIT_ASSERT(events[1] == Notifier::NE_RESET_DRAW_CYCLE);
   events = Notifier::Instance().GetEvents(pNotifyTarget3);
   CPPUNIT_ASSERT(events.size() == 2);
   CPPUNIT_ASSERT(events[0] == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(events[1] == Notifier::NE_RESET_DRAW_CYCLE);
   
}

void TestNotifier::TestInputArguments()
{
   RegisterTestObjects();
   CPPUNIT_ASSERT_THROW(Notifier::Instance().Attach(NULL, Notifier::NE_MIN), std::out_of_range);
   CPPUNIT_ASSERT_THROW(Notifier::Instance().Attach(pNotifyTarget1, (Notifier::NOTIFIED_EVENT_TYPE_T)(-1)), std::out_of_range);
   CPPUNIT_ASSERT_THROW(Notifier::Instance().Attach(pNotifyTarget1, Notifier::NE_MAX), std::out_of_range);
   CPPUNIT_ASSERT_THROW(Notifier::Instance().Detach(NULL, Notifier::NE_MIN), std::out_of_range);
   CPPUNIT_ASSERT_THROW(Notifier::Instance().Detach(pNotifyTarget1, (Notifier::NOTIFIED_EVENT_TYPE_T)(-1)),std::out_of_range);
   CPPUNIT_ASSERT_THROW(Notifier::Instance().Detach(pNotifyTarget1, Notifier::NE_MAX),std::out_of_range);
   CPPUNIT_ASSERT_THROW(Notifier::Instance().Detach(NULL), std::out_of_range);
}

// Verify we can fetch all the objects for each message type.
void TestNotifier::TestFetchObjectsForMessages()
{
   vector<Notified*> notified;
   vector<Notifier::NOTIFIED_EVENT_TYPE_T> events;
   
   for(Notifier::NOTIFIED_EVENT_TYPE_T event = Notifier::NE_MIN; event < Notifier::NE_MAX; event++)
   {
      notified = Notifier::Instance().GetNotified(event);
      CPPUNIT_ASSERT(notified.size() == 0);
   }
   
   // Add some events
   RegisterTestObjects();
   
   // Verify there are objects for all the messages we used.
   // If any new ones get added, this test will fail.
   for(Notifier::NOTIFIED_EVENT_TYPE_T event = Notifier::NE_MIN; event < Notifier::NE_MAX; event++)
   {
      notified = Notifier::Instance().GetNotified(event);
      switch(event)
      {
         case Notifier::NE_DEBUG_BUTTON_PRESSED:
            CPPUNIT_ASSERT(notified.size() == 2);
            break;
         case Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_RESET_DRAW_CYCLE:
            CPPUNIT_ASSERT(notified.size() == 2);
            break;
         default:
            CPPUNIT_ASSERT(false);
            break;
      }
   }
   
   // Verify the correct messages are there for each object.
   events = Notifier::Instance().GetEvents(pNotifyTarget1);
   CPPUNIT_ASSERT(events.size() == 2);
   CPPUNIT_ASSERT(events[0] == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(events[1] == Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS);
   events = Notifier::Instance().GetEvents(pNotifyTarget2);
   CPPUNIT_ASSERT(events.size() == 2);
   CPPUNIT_ASSERT(events[0] == Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY);
   CPPUNIT_ASSERT(events[1] == Notifier::NE_RESET_DRAW_CYCLE);
   events = Notifier::Instance().GetEvents(pNotifyTarget3);
   CPPUNIT_ASSERT(events.size() == 2);
   CPPUNIT_ASSERT(events[0] == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(events[1] == Notifier::NE_RESET_DRAW_CYCLE);
   
   // Now delete one of the ojects and verify that the others are
   // undisturbed.
   delete pNotifyTarget2;
   pNotifyTarget2 = NULL;  // So that the cleanup works as expected.
   
   // Verify there are objects for all the messages we used.
   for(Notifier::NOTIFIED_EVENT_TYPE_T event = Notifier::NE_MIN; event < Notifier::NE_MAX; event++)
   {
      notified = Notifier::Instance().GetNotified(event);
      switch(event)
      {
         case Notifier::NE_DEBUG_BUTTON_PRESSED:
            CPPUNIT_ASSERT(notified.size() == 2);
            break;
         case Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY:
            CPPUNIT_ASSERT(notified.size() == 0);
            break;
         case Notifier::NE_RESET_DRAW_CYCLE:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         default:
            break;
      }
   }
   
   // Verify the correct messages are there for each object.
   events = Notifier::Instance().GetEvents(pNotifyTarget1);
   CPPUNIT_ASSERT(events.size() == 2);
   CPPUNIT_ASSERT(events[0] == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(events[1] == Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS);
   events = Notifier::Instance().GetEvents(pNotifyTarget2);
   CPPUNIT_ASSERT(events.size() == 0);
   events = Notifier::Instance().GetEvents(pNotifyTarget3);
   CPPUNIT_ASSERT(events.size() == 2);
   CPPUNIT_ASSERT(events[0] == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(events[1] == Notifier::NE_RESET_DRAW_CYCLE);
}
// Verify if we delete an object, it is removed from the Notifier.
void TestNotifier::TestDeleteObject()
{
   vector<Notified*> notified;
   // Add some events
   RegisterTestObjects();
   
   // Verify there are objects for all the messages we used.
   // If any new ones get added, this test will fail.
   for(Notifier::NOTIFIED_EVENT_TYPE_T event = Notifier::NE_MIN; event < Notifier::NE_MAX; event++)
   {
      notified = Notifier::Instance().GetNotified(event);
      switch(event)
      {
         case Notifier::NE_DEBUG_BUTTON_PRESSED:
            CPPUNIT_ASSERT(notified.size() == 2);
            break;
         case Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_RESET_DRAW_CYCLE:
            CPPUNIT_ASSERT(notified.size() == 2);
            break;
         default:
            CPPUNIT_ASSERT(false);
            break;
      }
   }
   
}

// Verify if we Attach an object, it can receive Notify(...) calls.
void TestNotifier::TestAttach()
{
   vector<NotifyTarget::NOTIFY_PAIR_T> pairs;
   
   // Add some events
   RegisterTestObjects();

   // Send a notification.
   Notifier::Instance().Notify(Notifier::NE_DEBUG_BUTTON_PRESSED);
   
   // Verify all the interested parties got this.
   // NOTE:  This also takes care of checking that others
   // did NOT get it.
   pairs = pNotifyTarget1->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 1);
   CPPUNIT_ASSERT(pairs[0].eventData == NULL);
   CPPUNIT_ASSERT(pairs[0].eventType == Notifier::NE_DEBUG_BUTTON_PRESSED);
   pairs = pNotifyTarget2->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 0);
   pairs = pNotifyTarget3->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 1);
   CPPUNIT_ASSERT(pairs[0].eventType == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(pairs[0].eventData == NULL);
}

// Verify if we Attach an object, it can receive Notify(...) calls.
void TestNotifier::TestDoubleAttach()
{
   vector<NotifyTarget::NOTIFY_PAIR_T> pairs;
   
   // Add some events
   RegisterTestObjects();
   RegisterTestObjects();
   
   // Send a notification.
   Notifier::Instance().Notify(Notifier::NE_DEBUG_BUTTON_PRESSED);
   
   // Verify all the interested parties got this.
   // NOTE:  This also takes care of checking that others
   // did NOT get it.
   pairs = pNotifyTarget1->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 1);
   CPPUNIT_ASSERT(pairs[0].eventData == NULL);
   CPPUNIT_ASSERT(pairs[0].eventType == Notifier::NE_DEBUG_BUTTON_PRESSED);
   pairs = pNotifyTarget2->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 0);
   pairs = pNotifyTarget3->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 1);
   CPPUNIT_ASSERT(pairs[0].eventType == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(pairs[0].eventData == NULL);
}

// Verify if we Detach an object, it an object can no longer receive Notify calls.
void TestNotifier::TestDetach()
{
   vector<NotifyTarget::NOTIFY_PAIR_T> pairs;
   
   // Add some events
   RegisterTestObjects();
   
   // Detach pNotifyTarget from the target message.
   Notifier::Instance().Detach(pNotifyTarget3, Notifier::NE_DEBUG_BUTTON_PRESSED);
   
   // Send a notification.
   Notifier::Instance().Notify(Notifier::NE_DEBUG_BUTTON_PRESSED);
   
   // Verify all the interested parties got this.
   pairs = pNotifyTarget1->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 1);
   CPPUNIT_ASSERT(pairs[0].eventData == NULL);
   CPPUNIT_ASSERT(pairs[0].eventType == Notifier::NE_DEBUG_BUTTON_PRESSED);
   pairs = pNotifyTarget2->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 0);
   pairs = pNotifyTarget3->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 0);
   
   // Clear out any counts.
   ResetNotifyTargets();
   
   // Register again.
   RegisterTestObjects();
   
   // Send the notification and verify you get all the expected hits.
   Notifier::Instance().Notify(Notifier::NE_DEBUG_BUTTON_PRESSED);
   
   // Verify all the interested parties got this.
   // NOTE:  This also takes care of checking that others
   // did NOT get it.
   pairs = pNotifyTarget1->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 1);
   CPPUNIT_ASSERT(pairs[0].eventData == NULL);
   CPPUNIT_ASSERT(pairs[0].eventType == Notifier::NE_DEBUG_BUTTON_PRESSED);
   pairs = pNotifyTarget2->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 0);
   pairs = pNotifyTarget3->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 1);
   CPPUNIT_ASSERT(pairs[0].eventType == Notifier::NE_DEBUG_BUTTON_PRESSED);
   CPPUNIT_ASSERT(pairs[0].eventData == NULL);

   // Clear out any counts.
   ResetNotifyTargets();   
   
   // Now deregister pNotifyTarget3 entirely (not just the event).
   Notifier::Instance().Detach(pNotifyTarget3);
   
   // Send a notification.
   Notifier::Instance().Notify(Notifier::NE_DEBUG_BUTTON_PRESSED);
   
   // Verify all the interested parties got this.
   pairs = pNotifyTarget1->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 1);
   CPPUNIT_ASSERT(pairs[0].eventData == NULL);
   CPPUNIT_ASSERT(pairs[0].eventType == Notifier::NE_DEBUG_BUTTON_PRESSED);
   pairs = pNotifyTarget2->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 0);
   pairs = pNotifyTarget3->GetNotifications();
   CPPUNIT_ASSERT(pairs.size() == 0);
}
// Verify if a Notify(...) call leads to an object being deleted,
// it does not get a call to Notify(...).
void TestNotifier::TestDeleteViaNotify()
{
   Notifier& notifier = Notifier::Instance();
   vector<Notifier::NOTIFIED_EVENT_TYPE_T> events;
   vector<Notified*> notified;
   
   // Add some events
   RegisterTestObjects();
   
   NotifyTarget* pNotifyTargetOrg = pNotifyTarget2;

   // Now call a Notify on pNotifyTarget1 by using the
   // Notifier::NE_DEBUG_BUTTON_PRESSED event.
   CPPUNIT_ASSERT_NO_THROW(notifier.Notify(Notifier::NE_DEBUG_BUTTON_PRESSED,(void*)DeleteNotifyTarget3));

   // Verify that pNotifyTarget3 is NULL.
   CPPUNIT_ASSERT(pNotifyTarget3 == NULL);
   
   // Verify that the Notifier doesn't know about it.
   // Verify the correct messages are there for each object.
   events = notifier.GetEvents(pNotifyTarget1);
   CPPUNIT_ASSERT(events.size() == 2);
   events = notifier.GetEvents(pNotifyTargetOrg);
   CPPUNIT_ASSERT(events.size() == 2);
   events = notifier.GetEvents(pNotifyTarget3);
   CPPUNIT_ASSERT(events.size() == 0);
   
   for(Notifier::NOTIFIED_EVENT_TYPE_T event = Notifier::NE_MIN; event < Notifier::NE_MAX; event++)
   {
      notified = notifier.GetNotified(event);
      switch(event)
      {
         case Notifier::NE_DEBUG_BUTTON_PRESSED:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_RESET_DRAW_CYCLE:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         default:
            CPPUNIT_ASSERT(false);
            break;
      }
   }
   
}
// Verify Reset leaves the Notifier empty.
void TestNotifier::TestReset()
{
   vector<Notifier::NOTIFIED_EVENT_TYPE_T> events;
   vector<Notified*> notified;
   
   // First, veriy that these objects are registered for nothing.
   events = Notifier::Instance().GetEvents(pNotifyTarget1);
   CPPUNIT_ASSERT(events.size() == 0);
   events = Notifier::Instance().GetEvents(pNotifyTarget2);
   CPPUNIT_ASSERT(events.size() == 0);
   events = Notifier::Instance().GetEvents(pNotifyTarget3);
   CPPUNIT_ASSERT(events.size() == 0);
   
   for(Notifier::NOTIFIED_EVENT_TYPE_T event = Notifier::NE_MIN; event < Notifier::NE_MAX; event++)
   {
      notified = Notifier::Instance().GetNotified(event);
      CPPUNIT_ASSERT(notified.size() == 0);
   }
   
   // Add some events
   RegisterTestObjects();
   
   // Verify the correct messages are there for each object.
   events = Notifier::Instance().GetEvents(pNotifyTarget1);
   CPPUNIT_ASSERT(events.size() == 2);
   events = Notifier::Instance().GetEvents(pNotifyTarget2);
   CPPUNIT_ASSERT(events.size() == 2);
   events = Notifier::Instance().GetEvents(pNotifyTarget3);
   CPPUNIT_ASSERT(events.size() == 2);
   
   for(Notifier::NOTIFIED_EVENT_TYPE_T event = Notifier::NE_MIN; event < Notifier::NE_MAX; event++)
   {
      notified = Notifier::Instance().GetNotified(event);
      switch(event)
      {
         case Notifier::NE_DEBUG_BUTTON_PRESSED:
            CPPUNIT_ASSERT(notified.size() == 2);
            break;
         case Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY:
            CPPUNIT_ASSERT(notified.size() == 1);
            break;
         case Notifier::NE_RESET_DRAW_CYCLE:
            CPPUNIT_ASSERT(notified.size() == 2);
            break;
         default:
            CPPUNIT_ASSERT(false);
            break;
      }
   }

   // Call Reset
   Notifier::Instance().Reset();
   // Verify the correct messages are there for each object.
   events = Notifier::Instance().GetEvents(pNotifyTarget1);
   CPPUNIT_ASSERT(events.size() == 0);
   events = Notifier::Instance().GetEvents(pNotifyTarget2);
   CPPUNIT_ASSERT(events.size() == 0);
   events = Notifier::Instance().GetEvents(pNotifyTarget3);
   CPPUNIT_ASSERT(events.size() == 0);
   
   for(Notifier::NOTIFIED_EVENT_TYPE_T event = Notifier::NE_MIN; event < Notifier::NE_MAX; event++)
   {
      notified = Notifier::Instance().GetNotified(event);
      CPPUNIT_ASSERT(notified.size() == 0);
   }
}

