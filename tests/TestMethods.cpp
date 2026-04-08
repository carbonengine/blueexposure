// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestMethods.h"
#include "TestMultipleInterfaces.h"


TestMethods::TestMethods( IRoot* lockobj /*= nullptr */ ) :
	m_returnError( false )
{
}

bool TestMethods::MethodReturningBool( bool val )
{
	return val;
}

int TestMethods::MethodReturningInt( int val )
{
	return val;
}

uint32_t TestMethods::MethodReturningUnsignedInt( uint32_t val )
{
	return val;
}

float TestMethods::MethodReturningFloat( float val )
{
	return val;
}

double TestMethods::MethodReturningDouble( double val )
{
	return val;
}

std::string TestMethods::MethodReturningString( const std::string& val )
{
	return val;
}

const char* TestMethods::MethodReturningConstChar( const char* val )
{
	return val;
}

std::wstring TestMethods::MethodReturningWString( const std::wstring& val )
{
	return val;
}

const wchar_t* TestMethods::MethodReturningConstWChar( const wchar_t* val )
{
	return val;
}

Vector2 TestMethods::MethodReturningVector2( const Vector2& val )
{
	return val;
}

Vector2d TestMethods::MethodReturningVector2d( const Vector2d& val )
{
	return val;
}

Vector3 TestMethods::MethodReturningVector3( const Vector3& val )
{
	return val;
}

Vector3d TestMethods::MethodReturningVector3d( const Vector3d& val )
{
	return val;
}

Vector3i TestMethods::MethodReturningVector3i( const Vector3i& val )
{
	return val;
}

Vector4d TestMethods::MethodReturningVector4d( const Vector4d& val )
{
	return val;
}

#if BLUE_WITH_PYTHON
PyObject* TestMethods::MethodReturningPyObject( PyObject* val )
{
	return val;
}
#endif

BlueSharedString TestMethods::ConvertStringToSharedString( const char* string )
{
	return BlueSharedString( string );
}

std::string TestMethods::ConvertSharedStringToString( const BlueSharedString& string )
{
	return string.c_str();
}

BlueSharedStringW TestMethods::ConvertWStringToSharedStringW( const wchar_t* string )
{
	return BlueSharedStringW( string );
}

std::wstring TestMethods::ConvertSharedStringWToWString( const BlueSharedStringW& string )
{
	return string.c_str();
}

TestMethods::Enum TestMethods::MethodWithEnumParam( Enum val )
{
	return val;
}


TestMultipleInterfaces* TestMethods::MethodReturningBlueObject( TestMultipleInterfaces* val )
{
	return val;
}

TestMultipleInterfaces* TestMethods::LeakyFactoryFunction()
{
	TestMultipleInterfacesPtr p;
	p.CreateInstance();

	return p.Detach();
}

Be::Result<bool> TestMethods::BeResultArity0()
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity1( int arg1 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity2( int arg1, int arg2 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity3( int arg1, int arg2, int arg3 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity4( int arg1, int arg2, int arg3, int arg4 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity5( int arg1, int arg2, int arg3, int arg4, int arg5 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 )
{
	Be::Result<bool> result( !m_returnError );
	return result;
}


Be::Result<bool> TestMethods::BeResultArity1WithRef( int& arg1 )
{
	arg1 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity2WithRef( int arg1, int& arg2 )
{
	arg2 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity3WithRef( int arg1, int arg2, int& arg3 )
{
	arg3 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity4WithRef( int arg1, int arg2, int arg3, int& arg4 )
{
	arg4 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity5WithRef( int arg1, int arg2, int arg3, int arg4, int& arg5 )
{
	arg5 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity6WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int& arg6 )
{
	arg6 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity7WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int& arg7 )
{
	arg7 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity8WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int& arg8 )
{
	arg8 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}

Be::Result<bool> TestMethods::BeResultArity9WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int& arg9 )
{
	arg9 = 42;

	Be::Result<bool> result( !m_returnError );
	return result;
}


void TestMethods::VoidArity0()
{

}

void TestMethods::VoidArity1( int arg1 )
{

}

void TestMethods::VoidArity2( int arg1, int arg2 )
{

}

void TestMethods::VoidArity3( int arg1, int arg2, int arg3 )
{

}

void TestMethods::VoidArity4( int arg1, int arg2, int arg3, int arg4 )
{

}

void TestMethods::VoidArity5( int arg1, int arg2, int arg3, int arg4, int arg5 )
{

}

void TestMethods::VoidArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 )
{

}

void TestMethods::VoidArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 )
{

}

void TestMethods::VoidArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 )
{

}

void TestMethods::VoidArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 )
{

}

int TestMethods::IntArity0()
{
	return 0;
}

int TestMethods::IntArity1( int arg1 )
{
	return 1;
}

int TestMethods::IntArity2( int arg1, int arg2 )
{
	return 2;
}

int TestMethods::IntArity3( int arg1, int arg2, int arg3 )
{
	return 3;
}

int TestMethods::IntArity4( int arg1, int arg2, int arg3, int arg4 )
{
	return 4;
}

int TestMethods::IntArity5( int arg1, int arg2, int arg3, int arg4, int arg5 )
{
	return 5;
}

int TestMethods::IntArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 )
{
	return 6;
}

int TestMethods::IntArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 )
{
	return 7;
}

int TestMethods::IntArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 )
{
	return 8;
}

int TestMethods::IntArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 )
{
	return 9;
}

Be::Result<bool> TestMethods::SafeFactoryFunction0( TestMultipleInterfaces** out )
{
	if( m_returnError )
	{
		*out = nullptr;
		return Be::Result<bool>( false );
	}

	TestMultipleInterfacesPtr p;
	p.CreateInstance();

	if( !p )
	{
		return Be::Result<bool>( false );
	}

	*out = p.Detach();

	return Be::Result<bool>( true );
}

Be::Result<bool> TestMethods::SafeFactoryFunction1( int arg1, TestMultipleInterfaces** out )
{
	return SafeFactoryFunction0( out );
}

Be::Result<bool> TestMethods::SafeFactoryFunction2( int arg1, int arg2, TestMultipleInterfaces** out )
{
	return SafeFactoryFunction0( out );
}

Be::Result<bool> TestMethods::SafeFactoryFunction3( int arg1, int arg2, int arg3, TestMultipleInterfaces** out )
{
	return SafeFactoryFunction0( out );
}

Be::Result<bool> TestMethods::SafeFactoryFunction4( int arg1, int arg2, int arg3, int arg4, TestMultipleInterfaces** out )
{
	return SafeFactoryFunction0( out );
}

Be::Result<bool> TestMethods::SafeFactoryFunction5( int arg1, int arg2, int arg3, int arg4, int arg5, TestMultipleInterfaces** out )
{
	return SafeFactoryFunction0( out );
}

Be::Result<bool> TestMethods::SafeFactoryFunction6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, TestMultipleInterfaces** out )
{
	return SafeFactoryFunction0( out );
}

Be::Result<bool> TestMethods::SafeFactoryFunction7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, TestMultipleInterfaces** out )
{
	return SafeFactoryFunction0( out );
}

Be::Result<bool> TestMethods::SafeFactoryFunction8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, TestMultipleInterfaces** out )
{
	return SafeFactoryFunction0( out );
}

int TestMethods::MethodWithListOfIntParameter( const std::vector<int>& params )
{
	return (int)params.size();
}


int TestMethods::MethodWithListOfListOfIntParameter( const std::vector<std::vector<int>>& params )
{
	int result = 0;
	for( auto it = params.begin(); it != params.end(); ++it )
	{
		result += (int)it->size();
	}

	return result;
}

int TestMethods::MethodWithListOfVector3Parameter( const std::vector<Vector3>& params )
{
	return (int)params.size();
}

int TestMethods::MethodWithListOfVector3dParameter( const std::vector<Vector3d>& params )
{
	return (int)params.size();
}

int TestMethods::MethodWithListOfVector3iParameter( const std::vector<Vector3i>& params )
{
	return (int)params.size();
}

int TestMethods::MethodWithListOfVector4dParameter( const std::vector<Vector4d>& params )
{
	return (int)params.size();
}

std::vector<int> TestMethods::MethodReturningListOfInt( int val )
{
	std::vector<int> returnValue( val );
	for( int i = 0; i < val; ++i )
	{
		returnValue[i] = i;
	}

	return returnValue;
}

std::vector<Vector3> TestMethods::MethodReturningListOfVector3( int val )
{
	std::vector<Vector3> returnValue( val );
	for( int i = 0; i < val; ++i )
	{
		Vector3 x = { (float)i, (float)i, (float)i };
		returnValue[i] = x;
	}

	return returnValue;
}

std::vector<Vector3d> TestMethods::MethodReturningListOfVector3d( int val )
{
	std::vector<Vector3d> returnValue( val );
	for( int i = 0; i < val; ++i )
	{
		Vector3d x = { (double)i, (double)i, (double)i };
		returnValue[i] = x;
	}

	return returnValue;
}

std::vector<Vector3i> TestMethods::MethodReturningListOfVector3i( int val )
{
	std::vector<Vector3i> returnValue( val );
	for( int i = 0; i < val; ++i )
	{
		Vector3i x = { (int)i, (int)i, (int)i };
		returnValue[i] = x;
	}

	return returnValue;
}

std::vector<Vector4d> TestMethods::MethodReturningListOfVector4d( int val )
{
	std::vector<Vector4d> returnValue( val );
	for( int i = 0; i < val; ++i )
	{
		Vector4d x = { (double)i, (double)i, (double)i, (double)i };
		returnValue[i] = x;
	}

	return returnValue;
}

int TestMethods::MethodWithMapOfStringToIntParameter( const std::map<std::string, int>& val )
{
	return (int)val.size();
}

std::map<std::string, int> TestMethods::MethodReturningMapOfStringToInt( int val )
{
	std::map<std::string, int> returnValue;

	if( val < 1 )
	{
		return returnValue;
	}

	returnValue["one"] = 1;

	if( val < 2 )
	{
		return returnValue;
	}

	returnValue["two"] = 2;

	if( val < 3 )
	{
		return returnValue;
	}

	returnValue["three"] = 3;

	if( val < 4 )
	{
		return returnValue;
	}

	returnValue["four"] = 4;

	return returnValue;
}

