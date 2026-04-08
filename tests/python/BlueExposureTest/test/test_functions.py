# Copyright © 2026 CCP ehf.

import unittest
import BlueExposureTest


class TestFunctions(unittest.TestCase):
    """
    A set of test cases focusing on module function exposure.
    """

    """
    This section tests the functions with all supported data types. Each function
    takes in one parameter and simply returns it. This exercises the exposure code
    for each data type, both as a parameter and a return value.

    Note that there are separate tests for handling the promotion/error reporting
    for passing in values of varying types.
    """

    def testFunctionReturningBool(self):
        orgVal = False
        val = BlueExposureTest.FunctionReturningBool(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningInt(self):
        orgVal = 42
        val = BlueExposureTest.FunctionReturningInt(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningUnsignedInt(self):
        orgVal = 42
        val = BlueExposureTest.FunctionReturningUnsignedInt(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningFloat(self):
        orgVal = 3.14
        val = BlueExposureTest.FunctionReturningFloat(orgVal)
        self.assertAlmostEqual(val, orgVal, places=6)

    def testFunctionReturningDouble(self):
        orgVal = 2.71828182845904523536
        val = BlueExposureTest.FunctionReturningDouble(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningString(self):
        orgVal = "this is a test"
        val = BlueExposureTest.FunctionReturningString(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningConstChar(self):
        orgVal = "this is a test"
        val = BlueExposureTest.FunctionReturningConstChar(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningWString(self):
        orgVal = u"this is a test"
        val = BlueExposureTest.FunctionReturningWString(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningConstWChar(self):
        orgVal = u"this is a test"
        val = BlueExposureTest.FunctionReturningConstWChar(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningVector2(self):
        orgVal = (1, 2)
        val = BlueExposureTest.FunctionReturningVector2(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningVector2d(self):
        orgVal = (1.4142135623730951, 2.71828182845904523536)
        val = BlueExposureTest.FunctionReturningVector2d(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningVector3(self):
        orgVal = (1, 2, 3)
        val = BlueExposureTest.FunctionReturningVector3(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningVector3d(self):
        orgVal = (1.4142135623730951, 2.71828182845904523536, 3.141592653589793)
        val = BlueExposureTest.FunctionReturningVector3d(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningVector3i(self):
        orgVal = (1, 2, 3)
        val = BlueExposureTest.FunctionReturningVector3i(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningVector4(self):
        orgVal = (1, 2, 3, 4)
        val = BlueExposureTest.FunctionReturningVector4(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningVector4d(self):
        orgVal = (1.1, 2.2, 3.3, 4.4)
        val = BlueExposureTest.FunctionReturningVector4d(orgVal)
        self.assertEqual(val, orgVal)

    def testFunctionReturningPyObject(self):
        orgVal = BlueExposureTest.TestAttributes()
        val = BlueExposureTest.FunctionReturningPyObject(orgVal)
        self.assertEqual(val, orgVal)

    def testBoolFunction(self):
        """
        bool parameters should accept True, False and ints.
        Anything else should raise a type error.
        """

        val = BlueExposureTest.FunctionReturningBool(False)
        self.assertEqual(val, False)

        val = BlueExposureTest.FunctionReturningBool(0)
        self.assertEqual(val, False)

        val = BlueExposureTest.FunctionReturningBool(42)
        self.assertEqual(val, True)

        self.assertRaises(TypeError, BlueExposureTest.FunctionReturningBool, 3.14)
        self.assertRaises(TypeError, BlueExposureTest.FunctionReturningBool, "True")
        self.assertRaises(TypeError, BlueExposureTest.FunctionReturningBool, (1, 2))

    def testIntFunction(self):
        """
        int parameters should accept ints, True and False
        Anything else should raise a type error.
        """
        successValues = [0, 63, -1, 0x7fffffff, -0x7fffffff-1, True, False]
        typeErrorValues = [3.14, "this is not a number", "42", (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = BlueExposureTest.FunctionReturningInt(orgVal)
            self.assertEqual(type(val), int)
            self.assertEqual(val, orgVal, "Failed with parameter %d" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, BlueExposureTest.FunctionReturningInt, each)

    def testFloatFunction(self):
        """
        float parameters should accept floats and ints
        Anything else should raise a type error.
        """

        successValues = [0.0, 3.14, -2.77, 0, 42, -3, 2.71828182845904523536]
        typeErrorValues = ["this is not a number", "42", (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = BlueExposureTest.FunctionReturningFloat(orgVal)
            self.assertEqual(type(val), float)
            self.assertAlmostEqual(val, orgVal, msg="Failed with parameter %g" % each, places=6)

        for each in typeErrorValues:
            self.assertRaises(TypeError, BlueExposureTest.FunctionReturningFloat, each)

    def testDoubleFunction(self):
        """
        double parameters should accept floats and ints
        Anything else should raise a type error.
        """
        successValues = [0.0, 3.14, -2.77, 0, 42, -3, 2.71828182845904523536]
        typeErrorValues = ["this is not a number", "42", (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = BlueExposureTest.FunctionReturningDouble(orgVal)
            self.assertEqual(type(val), float)
            self.assertEqual(val, orgVal, msg="Failed with parameter %g" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, BlueExposureTest.FunctionReturningDouble, each)

    def testStringFunction(self):
        """
        string parameters should accept strings and unicode
        Anything else should raise a type error.
        """
        successValues = ["this is a string", u"this is unicode", "", u""]
        typeErrorValues = [True, False, 0, 42, 3.14, (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = BlueExposureTest.FunctionReturningString(orgVal)
            self.assertEqual(type(val), str)
            self.assertEqual(val, orgVal, msg="Failed with parameter %s" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, BlueExposureTest.FunctionReturningString, each)

    def testConstCharFunction(self):
        """
        string parameters should accept strings. Note that no conversion
        can take place from unicode - prefer std::string.
        Anything else should raise a type error.
        """
        successValues = ["this is a string", ""]
        typeErrorValues = [True, False, 0, 42, 3.14, (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = BlueExposureTest.FunctionReturningConstChar(orgVal)
            self.assertEqual(type(val), str)
            self.assertEqual(val, orgVal, msg="Failed with parameter %s" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, BlueExposureTest.FunctionReturningConstChar, each)

    def testUnicodeFunction(self):
        """
        unicode parameters should accept strings and unicode
        Anything else should raise a type error.
        """
        successValues = ["this is a string", u"this is unicode", "", u""]
        typeErrorValues = [True, False, 0, 42, 3.14, (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = BlueExposureTest.FunctionReturningWString(orgVal)
            self.assertEqual(type(val), unicode)
            self.assertEqual(val, orgVal, msg="Failed with parameter %s" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, BlueExposureTest.FunctionReturningWString, each)


    def testConstWCharFunction(self):
        """
        const wchar_t parameters should accept unicode. Note that no conversion
        can take place - prefer std::wstring.
        Anything else should raise a type error.
        """
        successValues = [u"this is unicode", u""]
        typeErrorValues = [True, False, 0, 42, 3.14, (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = BlueExposureTest.FunctionReturningConstWChar(orgVal)
            self.assertEqual(type(val), unicode)
            self.assertEqual(val, orgVal, msg="Failed with parameter %s" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, BlueExposureTest.FunctionReturningConstWChar, each)


    def testVoidArityFunctions(self):
        argsList = []

        for i in xrange(10):
            method = getattr(BlueExposureTest, "VoidArity%d" % i)
            args = tuple(argsList)

            self.assertEqual(method(*args), None)

            argsList.append(42)


    def testIntArityFunctions(self):
        argsList = []

        for i in xrange(10):
            method = getattr(BlueExposureTest, "IntArity%d" % i)
            args = tuple(argsList)

            self.assertEqual(method(*args), i)

            argsList.append(42)


    def testBeResultArityFunctions(self):
        argsList = []

        for i in xrange(10):
            BlueExposureTest.SetReturnError(False)

            method = getattr(BlueExposureTest, "BeResultArity%d" % i)
            args = tuple(argsList)

            self.assertEqual(method(*args), None)

            BlueExposureTest.SetReturnError(True)
            self.assertRaises(RuntimeError, method, *args)

            argsList.append(42)

    def testBeResultArityWithRefFunctions(self):
        argsList = []

        for i in xrange(9):
            BlueExposureTest.SetReturnError(False)

            method = getattr(BlueExposureTest, "BeResultArity%dWithRef" % (i+1))
            args = tuple(argsList)

            self.assertEqual(method(*args), 42)

            BlueExposureTest.SetReturnError(True)
            self.assertRaises(RuntimeError, method, *args)

            argsList.append(42)
