# Copyright © 2026 CCP ehf.

import unittest
import BlueExposureTest
import sys


class TestProperties(unittest.TestCase):
    """
    A set of test cases focusing on Blue exposure.
    """

    """
    Helper functions for basic assignment. These functions are called with an instance
    of either TestAttributes or TestProperties, and are expected to behave the same in
    either case. Pulling the assignments and assertions into these helper functions
    makes it easier to ensure we're doing the same tests for attributes and properties.
    """

    def _StringAssignment(self, x):
        x.myString = "this is a test"
        self.assertEqual(x.myString, "this is a test")

        x.myString = u"this is a test"
        self.assertEqual(x.myString, "this is a test")

    def _WStringAssignment(self, x):
        x.myUnicode = "this is a test"
        self.assertEqual(x.myUnicode, u"this is a test")

        x.myUnicode = u"this is a test"
        self.assertEqual(x.myUnicode, u"this is a test")

    def _BoolAssignment(self, x):
        x.myBool = True
        self.assertEqual(x.myBool, True)

        x.myBool = False
        self.assertEqual(x.myBool, False)

        x.myBool = 42
        self.assertEqual(x.myBool, True)

        x.myBool = 0
        self.assertEqual(x.myBool, False)

        def AssignFloat_ErrorExpected():
            x.myBool = 3.14
        self.assertRaises(TypeError, AssignFloat_ErrorExpected)

        def AssignString_ErrorExpected():
            x.myBool = "True"
        self.assertRaises(TypeError, AssignString_ErrorExpected)

    def _IntAssignment(self, x):
        values = [0, 63, -1, 0x7fffffff, -0x7fffffff-1, True, False]

        for each in values:
            x.myInt = each
            self.assertEqual(x.myInt, each, "Failed to assign %d" % each)

        def AssignLong_ErrorExpected():
            x.myInt = sys.maxint+1

        self.assertRaises(TypeError, AssignLong_ErrorExpected)

        def AssignFloat_ErrorExpected():
            x.myInt = 3.14

        self.assertRaises(TypeError, AssignFloat_ErrorExpected)

        def AssignString_ErrorExpected():
            x.myInt = "this is not a number"

        self.assertRaises(TypeError, AssignString_ErrorExpected)

    def _Int64Assignment(self, x):
        values = [0, 63, -1, 0x7fffffff, -0x7fffffff-1, 3L, 0x7fffffff*2, -0x7fffffff*2]

        for each in values:
            x.myInt64 = each
            self.assertEqual(x.myInt64, each)

        def AssignFloat_ErrorExpected():
            x.myInt64 = 3.14

        self.assertRaises(TypeError, AssignFloat_ErrorExpected)

        def AssignString_ErrorExpected():
            x.myInt64 = "this is not a number"

        self.assertRaises(TypeError, AssignString_ErrorExpected)

    def _FloatAssignment(self, x):
        values = [0.0, 3.14, -2.77, 0, 42, -3]

        for each in values:
            x.myFloat = each

            # Note that there are numerical precision issues here as the Blue
            # objects use floats rather than doubles
            self.assertAlmostEqual(x.myFloat, each, places=6)

        def AssignString_ErrorExpected():
            x.myFloat = "this is not a number"

        self.assertRaises(TypeError, AssignString_ErrorExpected)

    def _DoubleAssignment(self, x):
        values = [0.0, 3.14, -2.77, 0, 42, -3]

        for each in values:
            x.myDouble = each
            self.assertEqual(x.myDouble, each)

        def AssignString_ErrorExpected():
            x.myDouble = "this is not a number"

        self.assertRaises(TypeError, AssignString_ErrorExpected)

    def _BoolAssignment(self, x):
        x.myBool = True
        self.assertEqual(x.myBool, True)

        x.myBool = False
        self.assertEqual(x.myBool, False)

        x.myBool = 42
        self.assertEqual(x.myBool, True)

        x.myBool = 0
        self.assertEqual(x.myBool, False)

        def AssignFloat_ErrorExpected():
            x.myBool = 3.14

        self.assertRaises(TypeError, AssignFloat_ErrorExpected)

        def AssignString_ErrorExpected():
            x.myBool = "true"

        self.assertRaises(TypeError, AssignString_ErrorExpected)

    def _SharedStringAssignment(self, x):
        x.sharedString = "this is a test"
        self.assertEqual(x.sharedString, "this is a test")

        x.sharedString = u"this is a test"
        self.assertEqual(x.sharedString, "this is a test")


    """
    And here come the test functions themselves.
    """

    def testStringAttributeAssignment(self):
        x = BlueExposureTest.TestAttributes()
        self._StringAssignment(x)

    def testStringPropertyAssignment(self):
        x = BlueExposureTest.TestProperties()
        self._StringAssignment(x)

    def testWStringAttributeAssignment(self):
        x = BlueExposureTest.TestAttributes()
        self._WStringAssignment(x)

    def testWStringPropertyAssignment(self):
        x = BlueExposureTest.TestProperties()
        self._WStringAssignment(x)

    def testBoolAttributeAssignment(self):
        x = BlueExposureTest.TestAttributes()
        self._BoolAssignment(x)

    def testIntAttributeAssignment(self):
        x = BlueExposureTest.TestAttributes()
        self._IntAssignment(x)

    def testInt64AttributeAssignment(self):
        x = BlueExposureTest.TestAttributes()
        self._Int64Assignment(x)

    def testFloatAttributeAssignment(self):
        x = BlueExposureTest.TestAttributes()
        self._FloatAssignment(x)

    def testDoubleAttributeAssignment(self):
        x = BlueExposureTest.TestAttributes()
        self._DoubleAssignment(x)

    def testBoolPropertyAssignment(self):
        x = BlueExposureTest.TestProperties()
        self._BoolAssignment(x)

    def testIntPropertyAssignment(self):
        x = BlueExposureTest.TestProperties()
        self._IntAssignment(x)

    def testInt64PropertyAssignment(self):
        x = BlueExposureTest.TestProperties()
        self._Int64Assignment(x)

    def testFloatPropertyAssignment(self):
        x = BlueExposureTest.TestProperties()
        self._FloatAssignment(x)

    def testDoublePropertyAssignment(self):
        x = BlueExposureTest.TestProperties()
        self._DoubleAssignment(x)

    def testIntPropertyAssignmentWithError(self):
        x = BlueExposureTest.TestProperties()

        x.myIntWithError = 42
        self.assertEqual(x.myIntWithError, 42)

        def GetIt():
            return x.myIntWithError

        def SetIt(val):
            x.myIntWithError = val

        x.returnError = True
        self.assertRaises(RuntimeError, GetIt)
        self.assertRaises(RuntimeError, SetIt, 42)

    def testSharedStringAttributeAssignment(self):
        x = BlueExposureTest.TestAttributes()
        self._SharedStringAssignment(x)

    def testSharedStringPropertyAssignment(self):
        x = BlueExposureTest.TestProperties()
        self._SharedStringAssignment(x)

    def _verifyDir(self, x):
        members = dir(x)

        self.assertTrue( "myInt" in members )
        self.assertTrue( "myFloat" in members )
        self.assertTrue( "myDouble" in members )
        self.assertTrue( "myBool" in members )
        self.assertTrue( "myInt64" in members )
        self.assertTrue( "myString" in members )
        self.assertTrue( "myUnicode" in members )

    def testDirAttributes(self):
        x = BlueExposureTest.TestAttributes()

        self._verifyDir(x)

    def testDirProperties(self):
        x = BlueExposureTest.TestProperties()

        self._verifyDir(x)
