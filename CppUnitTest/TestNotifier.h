/********************************************************************
 * File   : TestNotifier.h
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

#ifndef __CppUnitTest__TestNotifier__
#define __CppUnitTest__TestNotifier__

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


/* Executes unit tests against the "Notifier" class.
 */
class TestNotifier : public CppUnit::TestFixture
{
   
public:
   TestNotifier();
   ~TestNotifier();
   
   // Verify we can fetch all the messages for an object.
   void TestFetchMessagesForObject();
   // Verify we can fetch all the objects for each message type.
   void TestFetchObjectsForMessages();
   // Verify if we delete an object, it is removed from the Notifier.
   void TestDeleteObject();
   // Verify if we Attach an object, it can receive Notify(...) calls.
   void TestAttach();
   // Verify if we Detach an object, it an object can no longer receive Notify calls.
   void TestDetach();
   // Verify if a Notify(...) call leads to an object being deleted,
   // it does not get a call to Notify(...).
   void TestDeleteViaNotify();
   // Verify Reset leaves the Notifier empty.
   void TestReset();
   // Verify arguments work correct.
   void TestInputArguments();
   // Verify if you register for the same even twice, you
   // only receive it once.
   void TestDoubleAttach();
   
   void setUp();
   void tearDown();
   
   
   
public:
   CPPUNIT_TEST_SUITE(TestNotifier);
   CPPUNIT_TEST(TestFetchMessagesForObject);
   CPPUNIT_TEST(TestFetchObjectsForMessages);
   CPPUNIT_TEST(TestDeleteObject);
   CPPUNIT_TEST(TestAttach);
   CPPUNIT_TEST(TestDoubleAttach);
   CPPUNIT_TEST(TestDetach);
   CPPUNIT_TEST(TestDeleteViaNotify);
   CPPUNIT_TEST(TestReset);
   CPPUNIT_TEST(TestInputArguments);
   CPPUNIT_TEST_SUITE_END();
   
};

#endif /* defined(__CppUnitTest__TestNotifier__) */
