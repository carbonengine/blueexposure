// Copyright © 2026 CCP ehf.

#include "BlueExposureMacros.h"
#include "BlueStdResult.h"

#if BLUE_WITH_PYTHON

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdIndexError )()
{
	return PyExc_IndexError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdKeyError )()
{
	return PyExc_KeyError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdAssertionError )()
{
	return PyExc_AssertionError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdAttributeError )()
{
	return PyExc_AttributeError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdEofError )()
{
	return PyExc_EOFError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdIOError )()
{
	return PyExc_IOError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdMemoryError )()
{
	return PyExc_MemoryError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdOSError )()
{
	return PyExc_OSError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdOverflowError )()
{
	return PyExc_OverflowError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdRuntimeError )()
{
	return PyExc_RuntimeError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdTypeError )()
{
	return PyExc_TypeError;
}

BLUEIMPORT PyObject* CCP_CONCATENATE( BlueGetException, BlueStdValueError )()
{
	return PyExc_ValueError;
}

#endif