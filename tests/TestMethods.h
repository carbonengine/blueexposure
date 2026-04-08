// Copyright © 2026 CCP ehf.

#pragma once
#ifndef TestMethods_h
#define TestMethods_h

#include "BlueExposure.h"

// Use TestMultipleInterfaces objects to help with testing factory methods. It's better to use
// an object that implements multiple interfaces so we uncover issues with ambiguous conversions
// to IRoot*.
BLUE_DECLARE( TestMultipleInterfaces );

BLUE_CLASS( TestMethods ) : public IRoot
{
public:
	EXPOSE_TO_BLUE();

	enum Enum
	{
		ONE,
		TWO,
		THREE,
	};

	TestMethods( IRoot* lockobj = nullptr );

	// Test functions for the various data types supported by Blue exposure.
	// Each of the following methods returns the value passed in. This expands the
	// template code for each possible data type.
	bool MethodReturningBool( bool val );
	int MethodReturningInt( int val );
	uint32_t MethodReturningUnsignedInt( uint32_t val );
	float MethodReturningFloat( float val );
	double MethodReturningDouble( double val );
	const char* MethodReturningConstChar( const char* val );
	std::string MethodReturningString( const std::string& val );
	const wchar_t* MethodReturningConstWChar( const wchar_t* val );
	std::wstring MethodReturningWString( const std::wstring& val );
	Vector2 MethodReturningVector2( const Vector2& val );
	Vector2d MethodReturningVector2d( const Vector2d& val );
	Vector3 MethodReturningVector3( const Vector3& val );
	Vector3d MethodReturningVector3d( const Vector3d& val );
	Vector3i MethodReturningVector3i( const Vector3i& val );
	Vector4d MethodReturningVector4d( const Vector4d& val );
#if BLUE_WITH_PYTHON
	PyObject* MethodReturningPyObject( PyObject* val );
#endif


	int MethodWithListOfIntParameter( const std::vector<int>& params );
	int MethodWithListOfListOfIntParameter( const std::vector<std::vector<int>>& params );
	int MethodWithListOfVector3Parameter( const std::vector<Vector3>& params );
	int MethodWithListOfVector3dParameter( const std::vector<Vector3d>& params );
	int MethodWithListOfVector3iParameter( const std::vector<Vector3i>& params );
	int MethodWithListOfVector4dParameter( const std::vector<Vector4d>& params );

	int MethodWithMapOfStringToIntParameter( const std::map<std::string, int>& val );

	std::vector<int> MethodReturningListOfInt( int val );
	std::vector<Vector3> MethodReturningListOfVector3( int val );
	std::vector<Vector3d> MethodReturningListOfVector3d( int val );
	std::vector<Vector3i> MethodReturningListOfVector3i( int val );
	std::vector<Vector4d> MethodReturningListOfVector4d( int val );
	
	std::map<std::string, int> MethodReturningMapOfStringToInt( int val );

	Enum MethodWithEnumParam( Enum val );

	// Note that this is not a factory method - it's returning a pointer to
	// an existing object. Caller is expected to add a reference - that's what
	// the wrapping code will do.
	TestMultipleInterfaces* MethodReturningBlueObject( TestMultipleInterfaces* val );

#if BLUE_WITH_PYTHON
	PyObject* PyMethodReturningBoolOrError( PyObject* args );
	PyObject* PyMethodAcceptingKeywordArguments( PyObject * args, PyObject * kwargs );
#endif

	BlueSharedString ConvertStringToSharedString( const char* string );
	std::string ConvertSharedStringToString( const BlueSharedString& string );

	BlueSharedStringW ConvertWStringToSharedStringW( const wchar_t* string );
	std::wstring ConvertSharedStringWToWString( const BlueSharedStringW& string );


	// Factory methods can easily cause reference leaks. Exposing a factory function
	// like this is guaranteed to leak references - DON'T DO THIS!
	TestMultipleInterfaces* LeakyFactoryFunction();

	// The correct way to expose factory methods is to take in a pointer pointer
	// and have the factory function populate it. Methods returning Be::Result
	// support the last parameter being a pointer to a pointer to an IRoot object
	// (or an IRoot descendant). When called from Python, this type of function
	// returns the last argument.
	Be::Result<bool> SafeFactoryFunction0( TestMultipleInterfaces** out );
	Be::Result<bool> SafeFactoryFunction1( int arg1, TestMultipleInterfaces** out );
	Be::Result<bool> SafeFactoryFunction2( int arg1, int arg2, TestMultipleInterfaces** out );
	Be::Result<bool> SafeFactoryFunction3( int arg1, int arg2, int arg3, TestMultipleInterfaces** out );
	Be::Result<bool> SafeFactoryFunction4( int arg1, int arg2, int arg3, int arg4, TestMultipleInterfaces** out );
	Be::Result<bool> SafeFactoryFunction5( int arg1, int arg2, int arg3, int arg4, int arg5, TestMultipleInterfaces** out );
	Be::Result<bool> SafeFactoryFunction6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, TestMultipleInterfaces** out );
	Be::Result<bool> SafeFactoryFunction7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, TestMultipleInterfaces** out );
	Be::Result<bool> SafeFactoryFunction8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, TestMultipleInterfaces** out );

	// Test the different arities of methods returning a Be::Result. These methods
	// return None to Python, or raise an exception if the Be::Result is not a success.
	Be::Result<bool> BeResultArity0();
	Be::Result<bool> BeResultArity1( int arg1 );
	Be::Result<bool> BeResultArity2( int arg1, int arg2 );
	Be::Result<bool> BeResultArity3( int arg1, int arg2, int arg3 );
	Be::Result<bool> BeResultArity4( int arg1, int arg2, int arg3, int arg4 );
	Be::Result<bool> BeResultArity5( int arg1, int arg2, int arg3, int arg4, int arg5 );
	Be::Result<bool> BeResultArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 );
	Be::Result<bool> BeResultArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 );
	Be::Result<bool> BeResultArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 );
	Be::Result<bool> BeResultArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 );

	// Test the different arities of methods returning a Be::Result where the last
	// parameter is a reference. The last parameter is returned to Python if the
	// Be::Result is a success, otherwise an exception is raised.
	Be::Result<bool> BeResultArity1WithRef( int& arg1 );
	Be::Result<bool> BeResultArity2WithRef( int arg1, int& arg2 );
	Be::Result<bool> BeResultArity3WithRef( int arg1, int arg2, int& arg3 );
	Be::Result<bool> BeResultArity4WithRef( int arg1, int arg2, int arg3, int& arg4 );
	Be::Result<bool> BeResultArity5WithRef( int arg1, int arg2, int arg3, int arg4, int& arg5 );
	Be::Result<bool> BeResultArity6WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int& arg6 );
	Be::Result<bool> BeResultArity7WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int& arg7 );
	Be::Result<bool> BeResultArity8WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int& arg8 );
	Be::Result<bool> BeResultArity9WithRef( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int& arg9 );

	// Test the different arities of void methods (return None to Python)
	void VoidArity0();
	void VoidArity1( int arg1 );
	void VoidArity2( int arg1, int arg2 );
	void VoidArity3( int arg1, int arg2, int arg3 );
	void VoidArity4( int arg1, int arg2, int arg3, int arg4 );
	void VoidArity5( int arg1, int arg2, int arg3, int arg4, int arg5 );
	void VoidArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 );
	void VoidArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 );
	void VoidArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 );
	void VoidArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 );

	// Test the different arities of methods returning a value
	int IntArity0();
	int IntArity1( int arg1 );
	int IntArity2( int arg1, int arg2 );
	int IntArity3( int arg1, int arg2, int arg3 );
	int IntArity4( int arg1, int arg2, int arg3, int arg4 );
	int IntArity5( int arg1, int arg2, int arg3, int arg4, int arg5 );
	int IntArity6( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 );
	int IntArity7( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7 );
	int IntArity8( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 );
	int IntArity9( int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9 );

private:
	bool m_returnError;
};

TYPEDEF_BLUECLASS( TestMethods );

#endif // TestMethods_h