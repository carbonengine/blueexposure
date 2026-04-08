// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueExtractArgument_h
#define BlueExtractArgument_h

#include "BlueVectorTypes.h"
#include "BlueTypeTraits.h"
#include <type_traits>
#include <typeinfo>
#include <string>

#if BLUE_WITH_PYTHON
#include "BlueExtractArgumentPython.h"
#endif

template< class POINTER_TYPE > POINTER_TYPE BluePythonCast( BlueScriptValue self );

// Template for Be::Optional types
template< typename T >
bool BlueExtractArgumentImpl( BlueScriptValue argument, Be::Optional<T>& result, unsigned int argID, std::true_type isBlueType )
{
	result.SetAssigned( true );
	return BlueExtractArgument( argument, result.GetValue(), argID );
}

// Template for Be::Optional types
template< typename T >
bool BlueExtractArgumentImpl( BlueScriptValue argument, Be::Optional<T>& result, unsigned int argID, std::false_type isBlueType )
{
	result.SetAssigned( true );
	return BlueExtractArgument( argument, result.GetValue(), argID );
}

// Remapping of const wchar_t* to std::wsting. This is to support automatic
// conversion of regular strings to wide strings.
class CastableStdWstring : public std::wstring
{
public:
	CastableStdWstring() : std::wstring() {}
	CastableStdWstring( const wchar_t* other ) : std::wstring( other ) {}
	operator const wchar_t*() const { return c_str(); }
};

template< typename T >
struct remap_wchar_to_string
{
	typedef T type;
};

template<>
struct remap_wchar_to_string<wchar_t* const>
{
	typedef CastableStdWstring type;
};

template<>
struct remap_wchar_to_string<const wchar_t*>
{
	typedef CastableStdWstring type;
};

template<>
struct remap_wchar_to_string<wchar_t*>
{
	typedef CastableStdWstring type;
};

// Overloads for specific types
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, float& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, double& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, int32_t& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, uint32_t& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, uint8_t& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, uint16_t& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, size_t& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, bool& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, std::string& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, const char*& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, std::wstring& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, const wchar_t*& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, int64_t& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, uint64_t& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Vector2& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Vector2d& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Vector3& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Vector3d& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Vector3i& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Vector4& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Vector4d& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Color& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Quaternion& result, unsigned int argID, std::false_type isBlueType );
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, Matrix& result, unsigned int argID, std::false_type isBlueType );

#ifdef _MSC_VER
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, unsigned long& result, unsigned int argID, std::false_type isBlueType );
#else
bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, long& result, unsigned int argID, std::false_type isBlueType );
#endif

// Special care for enums
template <typename T>
bool BlueExtractArgumentOrEnum( BlueScriptValue argument, T& resultRef, unsigned int argID, std::true_type isEnum )
{
	int result;
	if( BlueExtractArgumentImpl(
		argument,
		result,
		argID,
		std::false_type() ) )
	{
		resultRef = T( result );
		return true;
	}
	return false;
}

template <typename T>
bool BlueExtractArgumentOrEnum( BlueScriptValue argument, T& resultRef, unsigned int argID, std::false_type isEnum )
{
	// Do it differently if it's a blue type
	return BlueExtractArgumentImpl(
		argument,
		resultRef,
		argID,
		typename is_blue_type<T>::type());
}

//////////////////////////////////////////////////////////////////////////
// BlueExtractArgument
// Take a PyObject* argument for a function, and attempt to extract the type
// that matches the reference that was asked for.
//////////////////////////////////////////////////////////////////////////

template< typename T >
bool BlueExtractArgument( BlueScriptValue argument, T& resultRef, unsigned int argID )
{
	// Do it differently if it's a blue type
	return BlueExtractArgumentOrEnum(
		argument,
		resultRef,
		argID,
		typename std::is_enum<T>::type());
}



BLUEIMPORT bool BlueExtractString( BlueScriptValue obj, std::string& value );
BLUEIMPORT bool BlueExtractWString( BlueScriptValue obj, std::wstring& value );
BLUEIMPORT bool BlueExtractBool( BlueScriptValue obj, bool& value );
BLUEIMPORT bool BlueExtractInt( BlueScriptValue obj, int& value );
BLUEIMPORT bool BlueExtractFloat( BlueScriptValue obj, float& value );
BLUEIMPORT bool BlueExtractDouble( BlueScriptValue obj, double& value );
BLUEIMPORT bool BlueExtractVector( BlueScriptValue obj, float* elements, size_t elementsCount );
BLUEIMPORT bool BlueExtractMatrix( BlueScriptValue obj, float* elements, size_t elementsCount );

template<class T> bool BlueExtractVector( BlueScriptValue obj, T* elements, size_t elementsCount );

#if BLUE_WITH_PYTHON
// Conversion functions for passing objects to python
BLUEIMPORT PyObject* ConvertMatrixToSequence( const Matrix* m );
#endif

template< typename T >
void BlueGetNullValue( T& resultRef )
{
	resultRef = T( 0 );
}

inline void BlueGetNullValue( std::string& resultRef )
{
	resultRef = std::string();
}

inline void BlueGetNullValue( const char*& resultRef )
{
	resultRef = "";
}

inline void BlueGetNullValue( std::wstring& resultRef )
{
	resultRef = std::wstring();
}

inline void BlueGetNullValue( CastableStdWstring& resultRef )
{
	resultRef = CastableStdWstring();
}

inline void BlueGetNullValue( const wchar_t*& resultRef )
{
	resultRef = L"";
}

inline void BlueGetNullValue( Vector2& resultRef )
{
	// Don't use sizeof( Vector2 ) - modules may override the vector types if
	// BLUE_OVERRIDE_VECTOR_TYPES is defined, but may not provide definitions
	// for all vector types. That would then cause this code not to compile.
	// The vector types have to conform to a certain byte layout anyway so
	// using sizeof( float ) is safe.
	memset( &resultRef, 0, sizeof( float ) * 2 );
}

inline void BlueGetNullValue( Vector2d& resultRef )
{
	// Don't use sizeof( Vector2d )
	memset( &resultRef, 0, sizeof( double ) * 2 );
}

inline void BlueGetNullValue( Vector3& resultRef )
{
	// Don't use sizeof( Vector3 )
	memset( &resultRef, 0, sizeof( float ) * 3 );
}

inline void BlueGetNullValue( Vector3d& resultRef )
{
	// Don't use sizeof( Vector3d )
	memset( &resultRef, 0, sizeof( double ) * 3 );
}

inline void BlueGetNullValue( Vector3i& resultRef )
{
	// Don't use sizeof( Vector3i )
	memset( &resultRef, 0, sizeof( int ) * 3 );
}

inline void BlueGetNullValue( Vector4& resultRef )
{
	// Don't use sizeof( Vector4 )
	memset( &resultRef, 0, sizeof( float ) * 4 );
}

inline void BlueGetNullValue( Vector4d& resultRef )
{
	// Don't use sizeof( Vector4d )
	memset( &resultRef, 0, sizeof( double ) * 4 );
}

inline void BlueGetNullValue( Color& resultRef )
{
	// Don't use sizeof( Vector4d )
	memset( &resultRef, 0, sizeof( float ) * 4 );
}

#if BLUE_WITH_PYTHON
inline void BlueGetNullValue( PyObject*& resultRef )
{
	resultRef = Py_None;
}
#endif

template< typename T >
void BlueGetNullValue( Be::Optional<T>& resultRef )
{
	resultRef = Be::Optional<T>();
}

template< typename T, T defaultValue>
void BlueGetNullValue( Be::OptionalWithDefaultValue<T, defaultValue>& resultRef )
{
	resultRef = Be::OptionalWithDefaultValue<T, defaultValue>();
}

#endif // BlueExtractArgument_h