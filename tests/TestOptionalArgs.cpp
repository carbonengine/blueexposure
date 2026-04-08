// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestOptionalArgs.h"

bool TestOptionalArgs::MethodReturningBool( bool val )
{
	return val;
}

int TestOptionalArgs::MethodReturningInt( int val )
{
	return val;
}

uint32_t TestOptionalArgs::MethodReturningUnsignedInt( uint32_t val )
{
	return val;
}

float TestOptionalArgs::MethodReturningFloat( float val )
{
	return val;
}

double TestOptionalArgs::MethodReturningDouble( double val )
{
	return val;
}

std::string TestOptionalArgs::MethodReturningString( const std::string& val )
{
	return val;
}

const char* TestOptionalArgs::MethodReturningConstChar( const char* val )
{
	return val;
}

std::wstring TestOptionalArgs::MethodReturningWString( const std::wstring& val )
{
	return val;
}

const wchar_t* TestOptionalArgs::MethodReturningConstWChar( const wchar_t* val )
{
	return val;
}

Vector2 TestOptionalArgs::MethodReturningVector2( const Vector2& val )
{
	return val;
}

Vector2d TestOptionalArgs::MethodReturningVector2d( const Vector2d& val )
{
	return val;
}

Vector3 TestOptionalArgs::MethodReturningVector3( const Vector3& val )
{
	return val;
}

Vector3d TestOptionalArgs::MethodReturningVector3d( const Vector3d& val )
{
	return val;
}

Vector3i TestOptionalArgs::MethodReturningVector3i( const Vector3i& val )
{
	return val;
}

Vector4d TestOptionalArgs::MethodReturningVector4d( const Vector4d& val )
{
	return val;
}

#if BLUE_WITH_PYTHON
PyObject* TestOptionalArgs::MethodReturningPyObject( PyObject* val )
{
	return val;
}
#endif


TestMultipleInterfaces* TestOptionalArgs::MethodReturningBlueObject( TestMultipleInterfaces* val )
{
	return val;
}

int TestOptionalArgs::CalcSum( int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9 )
{
	return a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9;
}

bool TestOptionalArgs::BeOptionalIsSpecified( Be::Optional<int> val )
{
	return val.IsAssigned();
}

int TestOptionalArgs::BeOptional( Be::Optional<int> val )
{
	if( val.IsAssigned() )
	{
		return val;
	}
	else
	{
		return 0;
	}
}

int TestOptionalArgs::BeOptionalWithDefault( Be::OptionalWithDefaultValue<int, 123> val )
{
	return val;
}
