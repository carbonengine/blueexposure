// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestProperties.h"

BLUE_DEFINE( TestProperties );

const Be::ClassInfo* TestProperties::ExposeToBlue()
{
	EXPOSURE_BEGIN( TestProperties, "TestProperties is used to help test Blue exposure" )
		MAP_INTERFACE( TestProperties )

		MAP_ATTRIBUTE
		(
			"returnError",
			m_returnError,
			"If set, properties named xxxWithError raise a runtime error.",
			Be::READWRITE
		)

		MAP_PROPERTY
		( 
			"myString", 
			GetString, SetString, 
			"std::string test"
		)

		MAP_PROPERTY
		( 
			"myUnicode", 
			GetUnicode, SetUnicode, 
			"std::wstring test"
		)

		MAP_PROPERTY
		( 
			"myBool", 
			GetBool, SetBool, 
			"bool test"
		)

		MAP_PROPERTY
		( 
			"myInt", 
			GetInt, SetInt, 
			"int test"
		)

		MAP_PROPERTY
		( 
			"myUnsignedInt", 
			GetUnsignedInt, SetUnsignedInt, 
			"unsigned int test"
		)

		MAP_PROPERTY
		( 
			"myFloat", 
			GetFloat, SetFloat, 
			"float test"
		)

		MAP_PROPERTY
		( 
			"myDouble", 
			GetDouble, SetDouble, 
			"double test"
		)

		MAP_PROPERTY
		( 
			"myInt64", 
			GetInt64, SetInt64, 
			"int64_t test"
		)

		MAP_PROPERTY
		( 
			"myIntWithError", 
			GetIntWithError, SetIntWithError,
			"int test with error handling"
		)

		MAP_PROPERTY_READONLY
		(
			"myStringConst",
			GetStringConst,
			"const std::string test"
		)

		MAP_PROPERTY_READONLY
		(
			"myStringConstRef",
			GetStringConstRef,
			"const std::string& test"
		)

		MAP_PROPERTY
		( 
			"sharedString", 
			GetSharedString, SetSharedString,
			"shared string test"
		)

		MAP_PROPERTY
		(
			"sharedStringW",
			GetSharedStringW,
			SetSharedStringW,
			"shared stringw test" 
		)
	EXPOSURE_END()
}
