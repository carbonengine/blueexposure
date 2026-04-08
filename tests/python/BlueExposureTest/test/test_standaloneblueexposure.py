# Copyright © 2026 CCP ehf.

import unittest
import BlueExposureTest


class TestStandAloneBlueExposure(unittest.TestCase):
    """
    A set of test cases focusing on Blue exposure.
    """

    def testMethodCallsAndReturnValues(self):
        """
        This tests the methods with all supported data types. Each method takes
        in one parameter and simply returns it. This exercises the exposure code
        for each data type, both as a parameter and a return value.

        Note that there are separate tests for handling the promotion/error reporting
        for passing in values of varying types.
        """
        x = BlueExposureTest.TestMethods()

        orgVal = False
        val = x.MethodReturningBool(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = 42
        val = x.MethodReturningInt(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = 42
        val = x.MethodReturningUnsignedInt(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = 3.14
        val = x.MethodReturningFloat(orgVal)
        self.assertAlmostEqual(val, orgVal, places=6)

        orgVal = 2.71828182845904523536
        val = x.MethodReturningDouble(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = "this is a test"
        val = x.MethodReturningString(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = "this is a test"
        val = x.MethodReturningConstChar(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = u"this is a test"
        val = x.MethodReturningWString(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = u"this is a test"
        val = x.MethodReturningConstWChar(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = (1, 2)
        val = x.MethodReturningVector2(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = (1, 2, 3)
        val = x.MethodReturningVector3(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = x
        val = x.MethodReturningPyObject(orgVal)
        self.assertEqual(val, orgVal)

        self.assertTrue(x.MethodReturningBoolOrError())

        x.returnError = True
        self.assertRaises(RuntimeError, x.MethodReturningBoolOrError)

        orgVal = BlueExposureTest.TEST_ENUM.ONE
        val = x.MethodWithEnumParam(orgVal)
        self.assertEqual(val, orgVal)

        orgVal = BlueExposureTest.TEST_ENUM.TWO
        val = x.MethodWithEnumParam(orgVal)
        self.assertEqual(val, orgVal)

    def testBoolMethod(self):
        """
        bool parameters should accept True, False and ints.
        Anything else should raise a type error.
        """

        x = BlueExposureTest.TestMethods()

        val = x.MethodReturningBool(False)
        self.assertEqual(val, False)

        val = x.MethodReturningBool(0)
        self.assertEqual(val, False)

        val = x.MethodReturningBool(42)
        self.assertEqual(val, True)

        self.assertRaises(TypeError, x.MethodReturningBool, 3.14)
        self.assertRaises(TypeError, x.MethodReturningBool, "True")
        self.assertRaises(TypeError, x.MethodReturningBool, (1, 2))

    def testIntMethod(self):
        """
        int parameters should accept ints, True and False
        Anything else should raise a type error.
        """
        x = BlueExposureTest.TestMethods()

        successValues = [0, 63, -1, 0x7fffffff, -0x7fffffff-1, True, False]
        typeErrorValues = [3.14, "this is not a number", "42", (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = x.MethodReturningInt(orgVal)
            self.assertEqual(type(val), int)
            self.assertEqual(val, orgVal, "Failed with parameter %d" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, x.MethodReturningInt, each)

    def testFloatMethod(self):
        """
        float parameters should accept floats and ints
        Anything else should raise a type error.
        """
        x = BlueExposureTest.TestMethods()

        successValues = [0.0, 3.14, -2.77, 0, 42, -3, 2.71828182845904523536]
        typeErrorValues = ["this is not a number", "42", (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = x.MethodReturningFloat(orgVal)
            self.assertEqual(type(val), float)
            self.assertAlmostEqual(val, orgVal, msg="Failed with parameter %g" % each, places=6)

        for each in typeErrorValues:
            self.assertRaises(TypeError, x.MethodReturningFloat, each)

    def testDoubleMethod(self):
        """
        double parameters should accept floats and ints
        Anything else should raise a type error.
        """

        x = BlueExposureTest.TestMethods()

        successValues = [0.0, 3.14, -2.77, 0, 42, -3, 2.71828182845904523536]
        typeErrorValues = ["this is not a number", "42", (1, 2), [1, 2]]

        for each in successValues:
            orgVal = each
            val = x.MethodReturningDouble(orgVal)
            self.assertEqual(type(val), float)
            self.assertEqual(val, orgVal, msg="Failed with parameter %g" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, x.MethodReturningDouble, each)

    def testStringMethod(self):
        """
        string parameters should accept strings and unicode
        Anything else should raise a type error.
        """
        x = BlueExposureTest.TestMethods()

        successValues = ["this is a string", u"this is unicode", "", u""]
        typeErrorValues = [True, False, 0, 42, 3.14, (1, 2), [1, 2]]

        for each in successValues:
            orgVal = each
            val = x.MethodReturningString(orgVal)
            self.assertEqual(type(val), str)
            self.assertEqual(val, orgVal, msg="Failed with parameter %s" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, x.MethodReturningString, each)

    def testConstCharMethod(self):
        """
        string parameters should accept strings. Note that no conversion
        can take place from unicode - prefer std::string.
        Anything else should raise a type error.
        """
        x = BlueExposureTest.TestMethods()

        successValues = ["this is a string", ""]
        typeErrorValues = [True, False, 0, 42, 3.14, (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = x.MethodReturningConstChar(orgVal)
            self.assertEqual(type(val), str)
            self.assertEqual(val, orgVal, msg="Failed with parameter %s" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, x.MethodReturningConstChar, each)

    def testUnicodeMethod(self):
        """
        unicode parameters should accept strings and unicode
        Anything else should raise a type error.
        """
        x = BlueExposureTest.TestMethods()

        successValues = ["this is a string", u"this is unicode", "", u""]
        typeErrorValues = [True, False, 0, 42, 3.14, (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = x.MethodReturningWString(orgVal)
            self.assertEqual(type(val), unicode)
            self.assertEqual(val, orgVal, msg="Failed with parameter %s" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, x.MethodReturningWString, each)


    def testConstWCharMethod(self):
        """
        const wchar_t parameters should accept unicode. Note that no conversion
        can take place - prefer std::wstring.
        Anything else should raise a type error.
        """
        x = BlueExposureTest.TestMethods()

        successValues = [u"this is unicode", u""]
        typeErrorValues = [True, False, 0, 42, 3.14, (1, 2), [1, 2] ]

        for each in successValues:
            orgVal = each
            val = x.MethodReturningConstWChar(orgVal)
            self.assertEqual(type(val), unicode)
            self.assertEqual(val, orgVal, msg="Failed with parameter %s" % each)

        for each in typeErrorValues:
            self.assertRaises(TypeError, x.MethodReturningConstWChar, each)

    def testVoidArityMethods(self):
        x = BlueExposureTest.TestMethods()

        argsList = []

        for i in xrange(10):
            method = getattr(x, "VoidArity%d" % i)
            args = tuple(argsList)

            self.assertEqual(method(*args), None)

            argsList.append(42)

    def testIntArityMethods(self):
        x = BlueExposureTest.TestMethods()

        argsList = []

        for i in xrange(10):
            method = getattr(x, "IntArity%d" % i)
            args = tuple(argsList)

            self.assertEqual(method(*args), i)

            argsList.append(42)


    def testBeResultArityMethods(self):
        x = BlueExposureTest.TestMethods()

        argsList = []

        for i in xrange(10):
            x.returnError = False

            method = getattr(x, "BeResultArity%d" % i)
            args = tuple(argsList)

            self.assertEqual(method(*args), None)

            x.returnError = True
            self.assertRaises(RuntimeError, method, *args)

            argsList.append(42)


    def testBeResultArityWithRefMethods(self):
        x = BlueExposureTest.TestMethods()

        argsList = []

        for i in xrange(9):
            x.returnError = False

            method = getattr(x, "BeResultArity%dWithRef" % (i+1))
            args = tuple(argsList)

            self.assertEqual(method(*args), 42)

            x.returnError = True
            self.assertRaises(RuntimeError, method, *args)

            argsList.append(42)


    def testSafeFactory(self):
        x = BlueExposureTest.TestMethods()

        argsList = []

        lc = BlueExposureTest.classes.LiveCount()
        self.assertEqual(lc["_blueexposuretest.TestMultipleInterfaces"], 0, "Initial live count of helper object is not 0")

        for i in xrange(9):
            x.returnError = False

            method = getattr(x, "SafeFactoryFunction%d" % i)
            args = tuple(argsList)

            y = method(*args)
            self.assertEqual(type(y), BlueExposureTest.TestMultipleInterfaces)

            del y

            x.returnError = True
            self.assertRaises(RuntimeError, method, *args)

            argsList.append(42)

        lc = BlueExposureTest.classes.LiveCount()
        self.assertEqual(lc["_blueexposuretest.TestMultipleInterfaces"], 0, "Live count of helper object is not 0 after testing factory functions")


    def testWeakReferences(self):
        lc = BlueExposureTest.classes.LiveCount()
        self.assertEqual(lc["_blueexposuretest.TestReferences"], 0)

        x = BlueExposureTest.TestReferences()
        y = BlueExposureTest.TestReferences()

        x.weakRef = y
        self.assertEqual(x.weakRef, y)

        del y
        self.assertEqual(x.weakRef, None)

        x.weakRef = None
        self.assertEqual(x.weakRef, None)

        # Create a circular reference, with a weak ref. This shouldn't prevent
        # the object from dying.
        x.weakRef = x
        del x
        lc = BlueExposureTest.classes.LiveCount()
        self.assertEqual(lc["_blueexposuretest.TestReferences"], 0)


    def testMethodWithListOfIntParameter(self):
        x = BlueExposureTest.TestMethods()

        n = x.MethodWithListOfIntParameter([1, 2, 3])
        self.assertEqual(n, 3)

        n = x.MethodWithListOfIntParameter([])
        self.assertEqual(n, 0)

        n = x.MethodWithListOfIntParameter((1, 2, 3, 4, 5))
        self.assertEqual(n, 5)

        self.assertRaises(TypeError, x.MethodWithListOfIntParameter, 1)
        self.assertRaises(TypeError, x.MethodWithListOfIntParameter, [1, 2, "3"])
        self.assertRaises(TypeError, x.MethodWithListOfIntParameter, [1, 2, 3.14])


    def testMethodWithListOfListOfIntParameter(self):
        x = BlueExposureTest.TestMethods()

        n = x.MethodWithListOfListOfIntParameter([(1, 2, 3), (1, 2), (1, 2, 3, 4)])
        self.assertEqual(n, 9)

        n = x.MethodWithListOfListOfIntParameter([])
        self.assertEqual(n, 0)

        n = x.MethodWithListOfListOfIntParameter(((1, 2, 3), (1, 2), (1, 2, 3, 4)))
        self.assertEqual(n, 9)

        n = x.MethodWithListOfListOfIntParameter(([1, 2, 3], [1, 2], [1, 2, 3, 4]))
        self.assertEqual(n, 9)

        n = x.MethodWithListOfListOfIntParameter([[1, 2, 3], [1, 2], [1, 2, 3, 4]])
        self.assertEqual(n, 9)

    def testMethodWithListOfVector3Parameter(self):
        x = BlueExposureTest.TestMethods()

        n = x.MethodWithListOfVector3Parameter([(1, 1, 1), (2, 2, 2), (3, 3, 3)])
        self.assertEqual(n, 3)

        n = x.MethodWithListOfVector3Parameter([])
        self.assertEqual(n, 0)

        self.assertRaises(TypeError, x.MethodWithListOfVector3Parameter, [1, 2, "3"])
        self.assertRaises(TypeError, x.MethodWithListOfVector3Parameter, [1, 2, 3.14])


    def testMethodReturningListOfInt(self):
        x = BlueExposureTest.TestMethods()

        l = x.MethodReturningListOfInt(4)
        self.assertEqual(l, [0, 1, 2, 3])


    def testMethodReturningListOfVector3(self):
        x = BlueExposureTest.TestMethods()

        l = x.MethodReturningListOfVector3(2)
        self.assertEqual(l, [(0, 0, 0), (1, 1, 1)])


    def testMethodWithMapOfStringToIntParameter(self):
        x = BlueExposureTest.TestMethods()

        n = x.MethodWithMapOfStringToIntParameter({"one": 1, "two": 2})
        self.assertEqual(n, 2)

        n = x.MethodWithMapOfStringToIntParameter({})
        self.assertEqual(n, 0)

        self.assertRaises(TypeError, x.MethodWithMapOfStringToIntParameter, [1, 2, "3"])


    def testMethodReturningMapOfStringToInt(self):
        x = BlueExposureTest.TestMethods()

        d = x.MethodReturningMapOfStringToInt(2)
        self.assertEqual(d, {"one": 1, "two": 2})

        d = x.MethodReturningMapOfStringToInt(0)
        self.assertEqual(d, {})


    def testIsInstance(self):
        x = BlueExposureTest.TestMethods()

        self.assertTrue(isinstance(x, BlueExposureTest.TestMethods))
        self.assertTrue(isinstance(x, BlueExposureTest.BlueWrapper))
        self.assertTrue(issubclass(BlueExposureTest.TestMethods, BlueExposureTest.BlueWrapper))

    def testSharedStringMethods(self):
        x = BlueExposureTest.TestMethods()
        self.assertEqual(x.ConvertStringToSharedString("abc"), "abc")
        self.assertEqual(x.ConvertSharedStringToString("def"), "def")

    def testPassArgumentsWithoutKeywords(self):
        x = BlueExposureTest.TestMethods()
        result = x.MethodAcceptingKeywordArguments(1, 2, 3)
        self.assertEqual(6L, result)

    def testPassAllArgumentsWithKeywords(self):
        x = BlueExposureTest.TestMethods()
        result = x.MethodAcceptingKeywordArguments(a=1, b=2, c=3)
        self.assertEqual(6L, result)

    def testPassKeywordArgumentsWithKeywords(self):
        x = BlueExposureTest.TestMethods()
        result = x.MethodAcceptingKeywordArguments(1, b=2, c=3)
        self.assertEqual(6L, result)

    def testPassOneKeywordArgument(self):
        x = BlueExposureTest.TestMethods()
        result = x.MethodAcceptingKeywordArguments(1, b=2)
        self.assertEqual(3L, result)

    def testPassNoKeywordArgument(self):
        x = BlueExposureTest.TestMethods()
        result = x.MethodAcceptingKeywordArguments(1)
        self.assertEqual(1L, result)

    def testMissingRequiredArgument(self):
        x = BlueExposureTest.TestMethods()
        self.assertRaises(TypeError, x.MethodAcceptingKeywordArguments, b=1, c=2)
