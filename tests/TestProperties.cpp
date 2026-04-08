// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestProperties.h"

TestProperties::TestProperties( IRoot* lockobj ) :
	m_returnError( false ),
	m_myBool( false ),
	m_myInt( 0 ),
	m_myFloat( 0 ),
	m_myDouble( 0 ),
	m_myInt64( 0 )
{
}

bool TestProperties::GetBool() const
{
	return m_myBool;
}

void TestProperties::SetBool( bool val )
{
	m_myBool = val;
}

int TestProperties::GetInt() const
{
	return m_myInt;
}

void TestProperties::SetInt( int val )
{
	m_myInt = val;
}

uint32_t TestProperties::GetUnsignedInt() const
{
	return m_myUnsignedInt;
}

void TestProperties::SetUnsignedInt( uint32_t val )
{
	m_myUnsignedInt = val;
}

float TestProperties::GetFloat() const
{
	return m_myFloat;
}

void TestProperties::SetFloat( float val )
{
	m_myFloat = val;
}

double TestProperties::GetDouble() const
{
	return m_myDouble;
}

void TestProperties::SetDouble( double val )
{
	m_myDouble = val;
}

int64_t TestProperties::GetInt64() const
{
	return m_myInt64;
}

void TestProperties::SetInt64( int64_t val )
{
	m_myInt64 = val;
}

std::string TestProperties::GetString() const
{
	return m_myString;
}

void TestProperties::SetString( const std::string& val )
{
	m_myString = val;
}

std::wstring TestProperties::GetUnicode() const
{
	return m_myUnicode;
}

void TestProperties::SetUnicode( const std::wstring& val )
{
	m_myUnicode = val;
}

Be::Result<bool> TestProperties::GetIntWithError( int& val ) const
{
	val = m_myInt;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestProperties::SetIntWithError( int val )
{
	if( !m_returnError )
	{
		m_myInt = val;
	}
	Be::Result<bool> result( !m_returnError );
	return result;
}

const std::string TestProperties::GetStringConst() const
{
	return m_myString;
}

const std::string& TestProperties::GetStringConstRef() const
{
	return m_myString;
}

BlueSharedString TestProperties::GetSharedString() const
{
	return m_sharedString;
}

void TestProperties::SetSharedString( BlueSharedString string )
{
	m_sharedString = string;
}

BlueSharedStringW TestProperties::GetSharedStringW() const
{
	return m_sharedStringW;
}

void TestProperties::SetSharedStringW( BlueSharedStringW string )
{
	m_sharedStringW = string;
}
