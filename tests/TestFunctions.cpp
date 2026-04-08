// Copyright © 2026 CCP ehf.

#include "StdAfx.h"

// A flag to control whether functions returning a Be::Result should return
// errors or not. Used by unit testing to test error handling.
static bool s_returnError = false;

void SetReturnError( bool val )
{
	s_returnError = val;
}

MAP_FUNCTION_AND_WRAP( "SetReturnError", SetReturnError, "" );

bool FunctionReturningBool( bool val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningBool", FunctionReturningBool, "" );

#if BLUE_WITH_PYTHON
int FunctionReturningInt( int val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningInt", FunctionReturningInt, "" );

uint32_t FunctionReturningUnsignedInt( uint32_t val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningUnsignedInt", FunctionReturningUnsignedInt, "" );

float FunctionReturningFloat( float val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningFloat", FunctionReturningFloat, "" );

double FunctionReturningDouble( double val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningDouble", FunctionReturningDouble, "" );

const char* FunctionReturningConstChar( const char* val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningConstChar", FunctionReturningConstChar, "" );
#endif

std::string FunctionReturningString( const std::string& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningString", FunctionReturningString, "" );

#if BLUE_WITH_PYTHON
const wchar_t* FunctionReturningConstWChar( const wchar_t* val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningConstWChar", FunctionReturningConstWChar, "" );
#endif

std::wstring FunctionReturningWString( const std::wstring& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningWString", FunctionReturningWString, "" );

#if BLUE_WITH_PYTHON
Vector2 FunctionReturningVector2( const Vector2& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningVector2", FunctionReturningVector2, "" );

Vector2d FunctionReturningVector2d( const Vector2d& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningVector2d", FunctionReturningVector2d, "" );

Vector3 FunctionReturningVector3( const Vector3& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningVector3", FunctionReturningVector3, "" );

Vector3d FunctionReturningVector3d( const Vector3d& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningVector3d", FunctionReturningVector3d, "" );

Vector3i FunctionReturningVector3i( const Vector3i& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningVector3i", FunctionReturningVector3i, "" );

Vector4 FunctionReturningVector4( const Vector4& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningVector4", FunctionReturningVector4, "" );

Vector4d FunctionReturningVector4d( const Vector4d& val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningVector4d", FunctionReturningVector4d, "" );

#if BLUE_WITH_PYTHON
PyObject* FunctionReturningPyObject( PyObject* val )
{
	return val;
}

MAP_FUNCTION_AND_WRAP( "FunctionReturningPyObject", FunctionReturningPyObject, "" );
#endif


Be::Result<bool> BeResultArity0()
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity0", BeResultArity0, "" );

Be::Result<bool> BeResultArity1( int arg1 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity1", BeResultArity1, "" );

Be::Result<bool> BeResultArity2( int arg1, int arg2 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity2", BeResultArity2, "" );

Be::Result<bool> BeResultArity3( int arg1, int arg2, int arg3 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity3", BeResultArity3, "" );

Be::Result<bool> BeResultArity4( int arg1, int arg2, int arg3, int arg4 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity4", BeResultArity4, "" );

Be::Result<bool> BeResultArity5( int arg1, int arg2, int arg3, int arg4, int arg5 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity5", BeResultArity5, "" );

Be::Result<bool> BeResultArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity6", BeResultArity6, "" );

Be::Result<bool> BeResultArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity7", BeResultArity7, "" );

Be::Result<bool> BeResultArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity8", BeResultArity8, "" );

Be::Result<bool> BeResultArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 )
{
	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity9", BeResultArity9, "" );


Be::Result<bool> BeResultArity1WithRef( int& arg1 )
{
	arg1 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity1WithRef", BeResultArity1WithRef, "" );

Be::Result<bool> BeResultArity2WithRef( int arg1, int& arg2 )
{
	arg2 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity2WithRef", BeResultArity2WithRef, "" );

Be::Result<bool> BeResultArity3WithRef( int arg1, int arg2, int& arg3 )
{
	arg3 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity3WithRef", BeResultArity3WithRef, "" );

Be::Result<bool> BeResultArity4WithRef( int arg1, int arg2, int arg3, int& arg4 )
{
	arg4 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity4WithRef", BeResultArity4WithRef, "" );

Be::Result<bool> BeResultArity5WithRef( int arg1, int arg2, int arg3, int arg4, int& arg5 )
{
	arg5 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity5WithRef", BeResultArity5WithRef, "" );

Be::Result<bool> BeResultArity6WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int& arg6 )
{
	arg6 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity6WithRef", BeResultArity6WithRef, "" );

Be::Result<bool> BeResultArity7WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int& arg7 )
{
	arg7 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity7WithRef", BeResultArity7WithRef, "" );

Be::Result<bool> BeResultArity8WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int& arg8 )
{
	arg8 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity8WithRef", BeResultArity8WithRef, "" );

Be::Result<bool> BeResultArity9WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int& arg9 )
{
	arg9 = 42;

	Be::Result<bool> result( !s_returnError );
	return result;
}

MAP_FUNCTION_AND_WRAP( "BeResultArity9WithRef", BeResultArity9WithRef, "" );


void VoidArity0()
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity0", VoidArity0, "" );

void VoidArity1( int arg1 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity1", VoidArity1, "" );

void VoidArity2( int arg1, int arg2 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity2", VoidArity2, "" );

void VoidArity3( int arg1, int arg2, int arg3 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity3", VoidArity3, "" );

void VoidArity4( int arg1, int arg2, int arg3, int arg4 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity4", VoidArity4, "" );

void VoidArity5( int arg1, int arg2, int arg3, int arg4, int arg5 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity5", VoidArity5, "" );

void VoidArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity6", VoidArity6, "" );

void VoidArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity7", VoidArity7, "" );

void VoidArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity8", VoidArity8, "" );

void VoidArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 )
{
}

MAP_FUNCTION_AND_WRAP( "VoidArity9", VoidArity9, "" );


int IntArity0()
{
	return 0;
}

MAP_FUNCTION_AND_WRAP( "IntArity0", IntArity0, "" );

int IntArity1( int arg1 )
{
	return 1;
}

MAP_FUNCTION_AND_WRAP( "IntArity1", IntArity1, "" );

int IntArity2( int arg1, int arg2 )
{
	return 2;
}

MAP_FUNCTION_AND_WRAP( "IntArity2", IntArity2, "" );

int IntArity3( int arg1, int arg2, int arg3 )
{
	return 3;
}

MAP_FUNCTION_AND_WRAP( "IntArity3", IntArity3, "" );

int IntArity4( int arg1, int arg2, int arg3, int arg4 )
{
	return 4;
}

MAP_FUNCTION_AND_WRAP( "IntArity4", IntArity4, "" );

int IntArity5( int arg1, int arg2, int arg3, int arg4, int arg5 )
{
	return 5;
}

MAP_FUNCTION_AND_WRAP( "IntArity5", IntArity5, "" );

int IntArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 )
{
	return 6;
}

MAP_FUNCTION_AND_WRAP( "IntArity6", IntArity6, "" );

int IntArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 )
{
	return 7;
}

MAP_FUNCTION_AND_WRAP( "IntArity7", IntArity7, "" );

int IntArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 )
{
	return 8;
}

MAP_FUNCTION_AND_WRAP( "IntArity8", IntArity8, "" );

int IntArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 )
{
	return 9;
}

MAP_FUNCTION_AND_WRAP( "IntArity9", IntArity9, "" );

#endif