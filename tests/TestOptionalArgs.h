// Copyright © 2026 CCP ehf.

#pragma once
#ifndef TestOptionalArgs_h
#define TestOptionalArgs_h

#include "BlueExposure.h"

BLUE_DECLARE( TestMultipleInterfaces );

BLUE_CLASS( TestOptionalArgs ) : public IRoot
{
public:
	EXPOSE_TO_BLUE();

	// Test functions for the various data types supported by Blue exposure.
	// Each of the following methods returns the value passed in. This expands the
	// template code for each possible data type, and allows verification Python side
	// that the null value is as expected for optional arguments.

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

	TestMultipleInterfaces* MethodReturningBlueObject( TestMultipleInterfaces* val );

	int CalcSum( int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9 );

	bool BeOptionalIsSpecified( Be::Optional<int> val );
	int BeOptional( Be::Optional<int> val );
	int BeOptionalWithDefault( Be::OptionalWithDefaultValue<int, 123> val );
};

TYPEDEF_BLUECLASS( TestOptionalArgs );

#endif // TestOptionalArgs_h