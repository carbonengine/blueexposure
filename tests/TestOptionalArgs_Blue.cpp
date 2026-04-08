// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestOptionalArgs.h"
#include "TestMultipleInterfaces.h"

BLUE_DEFINE( TestOptionalArgs );

const Be::ClassInfo* TestOptionalArgs::ExposeToBlue()
{
	EXPOSURE_BEGIN( TestOptionalArgs, "TestOptionalArgs is used to help test Blue exposure" )
		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningBool",
			MethodReturningBool,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningInt",
			MethodReturningInt,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningUnsignedInt",
			MethodReturningUnsignedInt,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningFloat",
			MethodReturningFloat,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningDouble",
			MethodReturningDouble,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningConstChar",
			MethodReturningConstChar,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningString",
			MethodReturningString,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningConstWChar",
			MethodReturningConstWChar,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningWString",
			MethodReturningWString,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningVector2",
			MethodReturningVector2,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningVector3",
			MethodReturningVector3,
			1,
			""
		)

#if BLUE_WITH_PYTHON
		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningPyObject",
			MethodReturningPyObject,
			1,
			""
		)
#endif

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"MethodReturningBlueObject",
			MethodReturningBlueObject,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"CalcSum",
			CalcSum,
			8,
			"Calculates the sum of ints passed in, from 1 to 9."
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"BeOptionalIsSpecified",
			BeOptionalIsSpecified,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"BeOptional",
			BeOptional,
			1,
			""
		)

		MAP_METHOD_AND_WRAP_OPTIONAL_ARGS
		(
			"BeOptionalWithDefault",
			BeOptionalWithDefault,
			1,
			""
		)


	EXPOSURE_END()
}
