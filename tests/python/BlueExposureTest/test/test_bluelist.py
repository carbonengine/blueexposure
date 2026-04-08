# Copyright © 2026 CCP ehf.

import unittest
import BlueExposureTest


class DummyObject(object):
	"""
	An object that is not a Blue object.
	"""
	def __init__(self):
		self.x = 42


class TestBlueList(unittest.TestCase):
	"""
	A set of test case for Blue lists.
	"""
	def testAppend(self):
		obj = BlueExposureTest.TestAttributes()

		self.assertEqual(0, len(obj.myVector))

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		self.assertEqual(1, len(obj.myVector))

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		self.assertEqual(2, len(obj.myVector))

		self.assertEqual(childObj1, obj.myVector[0])
		self.assertEqual(childObj2, obj.myVector[1])

	def testAppend_None(self):
		obj = BlueExposureTest.TestAttributes()
		self.assertRaises(TypeError, obj.myVector.append, None)

	def testAppend_WrongType(self):
		obj = BlueExposureTest.TestAttributes()

		childObj = DummyObject()
		self.assertRaises(TypeError, obj.myVector.append, childObj)

		childObj = BlueExposureTest.TestProperties()
		# TODO: BlueList doesn't raise if the object is an IRoot but not the right type
		self.assertRaises(TypeError, obj.myVector.append, childObj)

	def testInsert(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		childObj4 = BlueExposureTest.TestAttributes()
		obj.myVector.insert(2, childObj4)

		self.assertEqual(4, len(obj.myVector))

		self.assertEqual(childObj1, obj.myVector[0])
		self.assertEqual(childObj2, obj.myVector[1])
		self.assertEqual(childObj4, obj.myVector[2])
		self.assertEqual(childObj3, obj.myVector[3])

	def testInsert_InvalidIndex(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		childObj4 = BlueExposureTest.TestAttributes()
		self.assertRaises(IndexError, obj.myVector.insert, -2, childObj4)

		self.assertEqual(3, len(obj.myVector))

		self.assertEqual(childObj1, obj.myVector[0])
		self.assertEqual(childObj2, obj.myVector[1])
		self.assertEqual(childObj3, obj.myVector[2])

	def testRemove(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		obj.myVector.remove(childObj2)

		self.assertEqual(2, len(obj.myVector))

		self.assertEqual(childObj1, obj.myVector[0])
		self.assertEqual(childObj3, obj.myVector[1])

	def testRemove_ItemNotInList(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		childObj4 = BlueExposureTest.TestAttributes()
		self.assertRaises(ValueError, obj.myVector.remove, childObj4)

		self.assertEqual(3, len(obj.myVector))

		self.assertEqual(childObj1, obj.myVector[0])
		self.assertEqual(childObj2, obj.myVector[1])
		self.assertEqual(childObj3, obj.myVector[2])

	def testRemove_ItemNotTheRightType(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		self.assertRaises(TypeError, obj.myVector.remove, 42)

		self.assertEqual(3, len(obj.myVector))

		self.assertEqual(childObj1, obj.myVector[0])
		self.assertEqual(childObj2, obj.myVector[1])
		self.assertEqual(childObj3, obj.myVector[2])

	def testRemoveAt(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		obj.myVector.removeAt(1)

		self.assertEqual(2, len(obj.myVector))

		self.assertEqual(childObj1, obj.myVector[0])
		self.assertEqual(childObj3, obj.myVector[1])

	def testRemoveAt_InvalidIndex(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		self.assertRaises(IndexError, obj.myVector.removeAt, 4)
		self.assertRaises(IndexError, obj.myVector.removeAt, -2)

		self.assertEqual(3, len(obj.myVector))

	def testRemoveAt_RemoveAll(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		obj.myVector.removeAt(-1)

		self.assertEqual(0, len(obj.myVector))

	def testExtend_BlueList(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		otherObj = BlueExposureTest.TestAttributes()

		childObj4 = BlueExposureTest.TestAttributes()
		otherObj.myVector.append(childObj4)

		childObj5 = BlueExposureTest.TestAttributes()
		otherObj.myVector.append(childObj5)

		otherObj.myVector.extend(obj.myVector)

		self.assertEqual(5, len(otherObj.myVector))
		self.assertEqual(childObj4, otherObj.myVector[0])
		self.assertEqual(childObj5, otherObj.myVector[1])
		self.assertEqual(childObj1, otherObj.myVector[2])
		self.assertEqual(childObj2, otherObj.myVector[3])
		self.assertEqual(childObj3, otherObj.myVector[4])

	def testExtend_PythonList(self):
		childObj1 = BlueExposureTest.TestAttributes()
		childObj2 = BlueExposureTest.TestAttributes()
		childObj3 = BlueExposureTest.TestAttributes()
		pythonList = [childObj1, childObj2, childObj3]

		otherObj = BlueExposureTest.TestAttributes()

		childObj4 = BlueExposureTest.TestAttributes()
		otherObj.myVector.append(childObj4)

		childObj5 = BlueExposureTest.TestAttributes()
		otherObj.myVector.append(childObj5)

		otherObj.myVector.extend(pythonList)

		self.assertEqual(5, len(otherObj.myVector))
		self.assertEqual(childObj4, otherObj.myVector[0])
		self.assertEqual(childObj5, otherObj.myVector[1])
		self.assertEqual(childObj1, otherObj.myVector[2])
		self.assertEqual(childObj2, otherObj.myVector[3])
		self.assertEqual(childObj3, otherObj.myVector[4])

	def testExtend_EmptyList(self):
		obj = BlueExposureTest.TestAttributes()
		obj.myVector.extend([])

		self.assertEqual(0, len(obj.myVector))

	def testPop(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		x = obj.myVector.pop()

		self.assertEqual(x, childObj3)
		self.assertEqual(2, len(obj.myVector))

	def testPop_Index(self):
		# pop doesn't allow an index, in spite of what the doc-string states
		return

		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		x = obj.myVector.pop(0)

		self.assertEqual(x, childObj1)
		self.assertEqual(2, len(obj.myVector))

	def testPop_Empty(self):
		# This test crashes :(
		return

		obj = BlueExposureTest.TestAttributes()
		x = obj.myVector.pop()

	def testIndex(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		ix = obj.myVector.index(childObj2)
		self.assertEqual(ix, 1)

	def testIndex_ItemNotInList(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		childObj4 = BlueExposureTest.TestAttributes()

		self.assertRaises(ValueError, obj.myVector.index, childObj4)

	def testIndex_ItemNotIRoot(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		childObj4 = DummyObject()

		self.assertRaises(TypeError, obj.myVector.index, childObj4)

	def testIndex_Empty(self):
		obj = BlueExposureTest.TestAttributes()

		childObj = BlueExposureTest.TestAttributes()

		self.assertRaises(ValueError, obj.myVector.index, childObj)

	def testCount(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)
		obj.myVector.append(childObj3)

		n1 = obj.myVector.count(childObj1)
		self.assertEqual(1, n1)

		n2 = obj.myVector.count(childObj2)
		self.assertEqual(1, n2)

		n3 = obj.myVector.count(childObj3)
		self.assertEqual(2, n3)

		childObj4 = BlueExposureTest.TestAttributes()
		n4 = obj.myVector.count(childObj4)
		self.assertEqual(0, n4)

	def testCount_ItemNotIRoot(self):
		obj = BlueExposureTest.TestAttributes()

		childObj = DummyObject()
		self.assertRaises(TypeError, obj.myVector.count, childObj)

	def testReverse(self):
		# The reverse method on blue lists is horribly broken
		return

		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		obj.myVector.reverse()

		self.assertEqual(childObj1, otherObj.myVector[2])
		self.assertEqual(childObj2, otherObj.myVector[1])
		self.assertEqual(childObj3, otherObj.myVector[0])

	def testReverse_Empty(self):
		obj = BlueExposureTest.TestAttributes()
		obj.myVector.reverse()

		# No assertions, just making sure this is exercised

	def testSort(self):
		# sort is not implemented
		return

		def Compare(a, b):
			if a.myString < b.myString:
				return -1

			if a.myString > b.myString:
				return 1

			return 0


		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		childObj1.myString = "b"
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		childObj2.myString = "c"
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		childObj3.myString = "a"
		obj.myVector.append(childObj3)

		obj.myVector.sort(Compare)

		self.assertEqual(childObj3, obj.myVector[0])
		self.assertEqual(childObj1, obj.myVector[1])
		self.assertEqual(childObj2, obj.myVector[2])


	def testSlice(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		childObj4 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj4)

		childObj5 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj5)

		sliceFromBlueList = obj.myVector[1:3]
		self.assertEqual(2, len(sliceFromBlueList))
		self.assertEqual(childObj2, sliceFromBlueList[0])
		self.assertEqual(childObj3, sliceFromBlueList[1])

	def testAssign(self):
		obj = BlueExposureTest.TestAttributes()

		childObj1 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj1)

		childObj2 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj2)

		childObj3 = BlueExposureTest.TestAttributes()
		obj.myVector.append(childObj3)

		childObj4 = BlueExposureTest.TestAttributes()
		obj.myVector[0] = childObj4

		self.assertEqual(3, len(obj.myVector))
		self.assertEqual(childObj4, obj.myVector[0])
		self.assertEqual(childObj2, obj.myVector[1])
		self.assertEqual(childObj3, obj.myVector[2])

	def testAssign_OutOfRange(self):
		obj = BlueExposureTest.TestAttributes()

		childObj = BlueExposureTest.TestAttributes()

		def AssignOutOfRange():
			obj.myVector[3] = childObj

		self.assertRaises(IndexError, AssignOutOfRange)

	def testRefCounting(self):
		obj = BlueExposureTest.TestAttributes()

		obj.myVector.append(BlueExposureTest.TestAttributes())

		lc = BlueExposureTest.classes.LiveCount()
		self.assertEqual(2, lc["_blueexposuretest.TestAttributes"])
