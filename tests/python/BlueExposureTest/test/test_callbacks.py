# Copyright © 2026 CCP ehf.

import unittest
import BlueExposureTest


def VoidCallback():
    pass


class TestCallbacks(unittest.TestCase):
    """
    A set of test cases focusing on Blue exposure's BlueScriptCallback.
    """

    def testScripCallbackInitializesToEmpty(self):
        x = BlueExposureTest.TestCallbacks()
        self.assertFalse(x.HasValidCallback())

    def testCallingEmptyCallbackReturnsException(self):
        x = BlueExposureTest.TestCallbacks()
        self.assertFalse(x.CallCallbackVoid())

    def testCanAssignCallback(self):
        x = BlueExposureTest.TestCallbacks()
        x.SetCallback(VoidCallback)
        self.assertTrue(x.HasValidCallback())
        self.assertEqual(x.GetCallback(), VoidCallback)

    def testCanCallVoidCallback(self):
        x = BlueExposureTest.TestCallbacks()
        called = [False,]

        def MyVoidCallback():
            called[0] = True

        x.SetCallback(MyVoidCallback)
        self.assertTrue(x.CallCallbackVoid())
        self.assertTrue(called[0])

    def testCallbackCatchesExceptions(self):
        x = BlueExposureTest.TestCallbacks()

        def MyVoidCallback():
            raise IndexError

        x.SetCallback(MyVoidCallback)
        self.assertFalse(x.CallCallbackVoid())

    def testCanCallIntCallback(self):
        x = BlueExposureTest.TestCallbacks()

        def MyIntCallback():
            return 123

        x.SetCallback(MyIntCallback)
        self.assertEqual(x.CallCallbackInt(), (True, 123))

    def testReturningWrongType(self):
        x = BlueExposureTest.TestCallbacks()

        def MyIntCallback():
            return "123"

        x.SetCallback(MyIntCallback)
        self.assertFalse(x.CallCallbackInt()[0])

    def testCanCallStringIntCallback(self):
        x = BlueExposureTest.TestCallbacks()

        def MyStringIntCallback(x):
            return str(x)

        x.SetCallback(MyStringIntCallback)
        self.assertEqual(x.CallCallbackStringInt(456), (True, "456"))

    def testCanCallIntStringFloatCallback(self):
        x = BlueExposureTest.TestCallbacks()

        def MyIntStringFloatCallback(x, y):
            return int(x) + int(y)

        x.SetCallback(MyIntStringFloatCallback)
        self.assertEqual(x.CallCallbackIntStringFloat("123", 456.4), (True, 579))

    def testCanSetCallbackProperty(self):
        x = BlueExposureTest.TestCallbacks()
        x.callback = VoidCallback
        self.assertEqual(x.callback, VoidCallback)

    def testCanSetCallbackAttribute(self):
        x = BlueExposureTest.TestCallbacks()
        x.callbackAttribute = VoidCallback
        self.assertEqual(x.callbackAttribute, VoidCallback)
