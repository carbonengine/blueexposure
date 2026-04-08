// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestMethods.h"
#include "TestMultipleInterfaces.h"

BLUE_DEFINE( TestMethods );

#if BLUE_WITH_PYTHON
PyObject* TestMethods::PyMethodReturningBoolOrError( PyObject* args )
{
	if( !PyArg_ParseTuple( args, "" ) )
	{
		return nullptr;
	}

	if( m_returnError )
	{
		PyErr_SetString( PyExc_RuntimeError, "returning an error" );
		return nullptr;
	}

	Py_RETURN_TRUE;
}

PyObject* TestMethods::PyMethodAcceptingKeywordArguments( PyObject* args, PyObject* kwargs )
{
	int a = 0;
	int b = 0;
	int c = 0;

	static const char* keywords[4] = {
		"a",
		"b",
		"c",
		nullptr
	};

	if( !PyArg_ParseTupleAndKeywords( args, kwargs, "i|ii", const_cast<char**>(keywords), &a, &b, &c ) )
		return nullptr;
	long result = a + b + c;
	return PyLong_FromLong(result);
}
#endif


bool TestSomething( std::true_type isPointerToPointerToBlue )
{
	return true;
}

bool TestSomething( std::false_type isPointerToPointerToBlue )
{
	return false;
}

#if BLUE_WITH_PYTHON
PyObject* PyTest( PyObject* self, PyObject* args )
{
	if( TestSomething( is_be_result<void*>::type() ) )
	{
		Py_RETURN_TRUE;
	}
	else
	{
		Py_RETURN_FALSE;
	}
}
#endif


Be::VarChooser TestMethodsEnumChooser[] =
{
	{
		"ONE",
		{ BeCast( TestMethods::ONE ) },
		"ONE"
	},
	{
		"TWO",
		{ BeCast( TestMethods::TWO ) },
		"TWO"
	},
	{
		"THREE",
		{ BeCast( TestMethods::THREE ) },
		"THREE"
	},
	{ 0 }
};

BLUE_REGISTER_ENUM_EX( 
    "TEST_ENUM", 
	TestMethods::Enum, 
    TestMethodsEnumChooser,
    ENUM_REG_ENUM_OBJECT_ON_MODULE
);

const Be::ClassInfo* TestMethods::ExposeToBlue()
{
	EXPOSURE_BEGIN( TestMethods, "TestMethods is used to help test Blue exposure" )
		MAP_INTERFACE( TestMethods )

		MAP_ATTRIBUTE
		(
			"returnError",
			m_returnError,
			"If set, methods return error rather than a value.",
			Be::READWRITE
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningBool",
			MethodReturningBool,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningInt",
			MethodReturningInt,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningUnsignedInt",
			MethodReturningUnsignedInt,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningFloat",
			MethodReturningFloat,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningDouble",
			MethodReturningDouble,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningString",
			MethodReturningString,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningConstChar",
			MethodReturningConstChar,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningWString",
			MethodReturningWString,
			""
		)

#if BLUE_WITH_PYTHON
		MAP_METHOD_AND_WRAP
		(
			"MethodReturningConstWChar",
			MethodReturningConstWChar,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningVector2",
			MethodReturningVector2,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningVector3",
			MethodReturningVector3,
			""
		)

#if BLUE_WITH_PYTHON
		MAP_METHOD_AND_WRAP
		(
			"MethodReturningPyObject",
			MethodReturningPyObject,
			""
		)
#endif

		MAP_METHOD_AND_WRAP
		(
			"ConvertStringToSharedString",
			ConvertStringToSharedString,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"ConvertSharedStringToString",
			ConvertSharedStringToString,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"ConvertWStringToSharedStringW",
			ConvertWStringToSharedStringW,
			"" 
		)

		MAP_METHOD_AND_WRAP
		(
			"ConvertSharedStringWToWString",
			ConvertSharedStringWToWString,
			"" 
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodWithEnumParam",
			MethodWithEnumParam,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningBlueObject",
			MethodReturningBlueObject,
			""
		)

		MAP_METHOD_AS_METHOD
		(
			"MethodReturningBoolOrError",
			PyMethodReturningBoolOrError,
			""
		)

		MAP_METHOD_WITH_KEYWORD_ARGUMENTS
		(
			"MethodAcceptingKeywordArguments",
			PyMethodAcceptingKeywordArguments,
			"Returns a + b + c"
		)

		MAP_METHOD_AND_WRAP
		(
			"LeakyFactoryFunction",
			LeakyFactoryFunction,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction0",
			SafeFactoryFunction0,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction1",
			SafeFactoryFunction1,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction2",
			SafeFactoryFunction2,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction3",
			SafeFactoryFunction3,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction4",
			SafeFactoryFunction4,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction5",
			SafeFactoryFunction5,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction6",
			SafeFactoryFunction6,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction7",
			SafeFactoryFunction7,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"SafeFactoryFunction8",
			SafeFactoryFunction8,
			""
		)
#endif
		MAP_METHOD_AND_WRAP
		(
			"BeResultArity0",
			BeResultArity0,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity1",
			BeResultArity1,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity2",
			BeResultArity2,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity3",
			BeResultArity3,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity4",
			BeResultArity4,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity5",
			BeResultArity5,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity6",
			BeResultArity6,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity7",
			BeResultArity7,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity8",
			BeResultArity8,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity9",
			BeResultArity9,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity1WithRef",
			BeResultArity1WithRef,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity2WithRef",
			BeResultArity2WithRef,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity3WithRef",
			BeResultArity3WithRef,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity4WithRef",
			BeResultArity4WithRef,
			""
		)
		
		MAP_METHOD_AND_WRAP
		(
			"BeResultArity5WithRef",
			BeResultArity5WithRef,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity6WithRef",
			BeResultArity6WithRef,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity7WithRef",
			BeResultArity7WithRef,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity8WithRef",
			BeResultArity8WithRef,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"BeResultArity9WithRef",
			BeResultArity9WithRef,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity0",
			VoidArity0,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity1",
			VoidArity1,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity2",
			VoidArity2,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity3",
			VoidArity3,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity4",
			VoidArity4,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity5",
			VoidArity5,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity6",
			VoidArity6,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity7",
			VoidArity7,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity8",
			VoidArity8,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"VoidArity9",
			VoidArity9,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity0",
			IntArity0,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity1",
			IntArity1,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity2",
			IntArity2,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity3",
			IntArity3,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity4",
			IntArity4,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity5",
			IntArity5,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity6",
			IntArity6,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity7",
			IntArity7,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity8",
			IntArity8,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"IntArity9",
			IntArity9,
			""
		)

#if BLUE_WITH_PYTHON
		MAP_METHOD_AND_WRAP
		(
			"MethodWithListOfIntParameter",
			MethodWithListOfIntParameter,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodWithListOfListOfIntParameter",
			MethodWithListOfListOfIntParameter,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodWithListOfVector3Parameter",
			MethodWithListOfVector3Parameter,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningListOfInt",
			MethodReturningListOfInt,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningListOfVector3",
			MethodReturningListOfVector3,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodWithMapOfStringToIntParameter",
			MethodWithMapOfStringToIntParameter,
			""
		)

		MAP_METHOD_AND_WRAP
		(
			"MethodReturningMapOfStringToInt",
			MethodReturningMapOfStringToInt,
			""
		)
#endif
	EXPOSURE_END()
}
