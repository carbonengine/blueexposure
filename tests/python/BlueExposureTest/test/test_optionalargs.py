# Copyright © 2026 CCP ehf.

import unittest
import BlueExposureTest


class TestOptionalArgs(unittest.TestCase):
    """
    A set of test cases focusing on optional arguments to methods.
    """

    def testMethodCallsAndReturnValues(self):
        """
        This tests the methods with all supported data types. Each method takes
        in one parameter and simply returns it. This exercises the exposure code
        for each data type, both as a parameter and a return value.
        """

        x = BlueExposureTest.TestOptionalArgs()

        orgVal = False
        val = x.MethodReturningBool(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningBool()
        self.assertEqual(val, False)

        orgVal = 42
        val = x.MethodReturningInt(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningInt()
        self.assertEqual(val, 0)

        orgVal = 42
        val = x.MethodReturningUnsignedInt(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningUnsignedInt()
        self.assertEqual(val, 0)

        orgVal = 3.14
        val = x.MethodReturningFloat(orgVal)
        self.assertAlmostEqual(val, orgVal, places=6)
        val = x.MethodReturningFloat()
        self.assertAlmostEqual(val, 0.0, places=6)

        orgVal = 2.71828182845904523536
        val = x.MethodReturningDouble(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningDouble()
        self.assertEqual(val, 0.0)

        orgVal = "this is a test"
        val = x.MethodReturningString(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningString()
        self.assertEqual(val, "")

        orgVal = "this is a test"
        val = x.MethodReturningConstChar(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningConstChar()
        self.assertEqual(val, "")

        orgVal = u"this is a test"
        val = x.MethodReturningWString(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningWString()
        self.assertEqual(val, u"")

        orgVal = u"this is a test"
        val = x.MethodReturningConstWChar(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningConstWChar()
        self.assertEqual(val, u"")

        orgVal = (1, 2)
        val = x.MethodReturningVector2(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningVector2()
        self.assertEqual(val, (0, 0))

        orgVal = (1, 2, 3)
        val = x.MethodReturningVector3(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningVector3()
        self.assertEqual(val, (0, 0, 0))

        orgVal = object()
        val = x.MethodReturningPyObject(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningPyObject()
        self.assertEqual(val, None)

        orgVal = BlueExposureTest.TestMultipleInterfaces()
        val = x.MethodReturningBlueObject(orgVal)
        self.assertEqual(val, orgVal)
        val = x.MethodReturningBlueObject()
        self.assertEqual(val, None)

    def testOptionalArgs(self):
        x = BlueExposureTest.TestOptionalArgs()

        self.assertRaises(TypeError, x.CalcSum)
        self.assertRaises(TypeError, x.CalcSum, None)

        result = x.CalcSum(1)
        self.assertEqual(result, 1)

        result = x.CalcSum(1, 2)
        self.assertEqual(result, 3)

        result = x.CalcSum(1, 2, 3)
        self.assertEqual(result, 6)

        result = x.CalcSum(1, 2, 3, 4)
        self.assertEqual(result, 10)

        result = x.CalcSum(1, 2, 3, 4, 5)
        self.assertEqual(result, 15)

        result = x.CalcSum(1, 2, 3, 4, 5, 6)
        self.assertEqual(result, 21)

        result = x.CalcSum(1, 2, 3, 4, 5, 6, 7)
        self.assertEqual(result, 28)

        result = x.CalcSum(1, 2, 3, 4, 5, 6, 7, 8)
        self.assertEqual(result, 36)

        result = x.CalcSum(1, 2, 3, 4, 5, 6, 7, 8, 9)
        self.assertEqual(result, 45)

        self.assertRaises(TypeError, x.CalcSum, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)

    def testBeOptionalArgs(self):
        x = BlueExposureTest.TestOptionalArgs()

        result = x.BeOptionalIsSpecified(1)
        self.assertEqual(result, True)

        result = x.BeOptionalIsSpecified()
        self.assertEqual(result, False)

        result = x.BeOptional()
        self.assertEqual(result, 0)

        result = x.BeOptional(0)
        self.assertEqual(result, 0)

        result = x.BeOptional(1)
        self.assertEqual(result, 1)

        result = x.BeOptionalWithDefault()
        self.assertEqual(result, 123)

        result = x.BeOptionalWithDefault(0)
        self.assertEqual(result, 0)

        result = x.BeOptionalWithDefault(1)
        self.assertEqual(result, 1)
