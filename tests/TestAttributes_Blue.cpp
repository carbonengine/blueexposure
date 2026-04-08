// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestAttributes.h"

BLUE_DEFINE( TestAttributes );

const Be::ClassInfo* TestAttributes::ExposeToBlue()
{
	EXPOSURE_BEGIN( TestAttributes, "TestAttributes is used to help test Blue exposure" )
		MAP_INTERFACE( TestAttributes )

		MAP_ATTRIBUTE
		( 
			"myString", 
			m_myString, 
			"std::string test", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		( 
			"myUnicode", 
			m_myUnicode, 
			"std::wstring test", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		( 
			"myBool", 
			m_myBool, 
			"bool test", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		( 
			"myInt", 
			m_myInt, 
			"int test", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		( 
			"myFloat", 
			m_myFloat, 
			"float test", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		( 
			"myDouble", 
			m_myDouble, 
			"double test", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		( 
			"myInt64", 
			m_myInt64, 
			"int64_t test", 
			Be::READWRITE | Be::PERSIST
		)

		MAP_ATTRIBUTE
		(
			"myVector",
			m_myVector,
			"vector test",
			Be::READ | Be::PERSIST
		)

#if BLUE_WITH_PYTHON
		MAP_ATTRIBUTE
		(
			"myDict",
			m_myDict,
			"dict test",
			Be::READ | Be::PERSIST
		)
#endif

		MAP_ATTRIBUTE
		(
			"sharedString",
			m_sharedString,
			"shared string test",
			Be::READWRITE | Be::PERSIST
		)
		MAP_ATTRIBUTE
		(
			"sharedStringW",
			m_sharedStringW,
			"shared string test",
			Be::READWRITE | Be::PERSIST 
		)
	EXPOSURE_END()
}

