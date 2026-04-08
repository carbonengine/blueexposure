// Copyright © 2026 CCP ehf.

#include "include/BlueSharedString.h"
#include "include/BlueExtractArgument.h"
#include "include/BlueWrapReturnValue.h"
#include "include/BlueStringTable.h"

using namespace std;

namespace
{

BlueStringTable<char>& GetCharPool()
{
	static BlueStringTable<char> s_pool;
	return s_pool;
}

BlueStringTable<wchar_t>& GetWCharPool()
{
	static BlueStringTable<wchar_t> s_pool;
	return s_pool;
}

CcpMutex& GetPoolMutex()
{
	static CcpMutex s_mutex( "SharedString", "SharedStringMutex", 300 );
	return s_mutex;
}

}

// --------------------------------------------------------------------------------------
// Description:
//   Allocates a string in a shared pool.
// Arguments:
//   string - zero terminated string
// Return value:
//   string in the memory pool which contents is equal to the passed string
// --------------------------------------------------------------------------------------
BLUEIMPORT const char* BlueAllocateSharedStringFromPool( const char* string )
{
	if( !string || !string[0] )
	{
		static const char* const EMPTY_STRING = "";
		return EMPTY_STRING;
	}
	CcpAutoMutex lock( GetPoolMutex() );
	return GetCharPool().GetString( string );
}

// --------------------------------------------------------------------------------------
// Description:
//   Allocates a wide string in a shared pool.
// Arguments:
//   string - zero terminated wide string
// Return value:
//   string in the memory pool which contents is equal to the passed string
// --------------------------------------------------------------------------------------
BLUEIMPORT const wchar_t* BlueAllocateSharedStringFromPool( const wchar_t* string )
{
	if( !string || !string[0] )
	{
		static const wchar_t* const EMPTY_STRING = L"";
		return EMPTY_STRING;
	}
	CcpAutoMutex lock( GetPoolMutex() );
	return GetWCharPool().GetString( string );
}

// --------------------------------------------------------------------------------------
// Description:
//   Returns basic statistics on shared string pools.
// Arguments:
//   charPoolCount - (out) Number of strings in the char string pool
//   charPoolSize - (out) Total memory size ofstrings in the char string pool
//   wcharPoolCount - (out) Number of strings in the wchar string pool
//   wcharPoolSize - (out) Total memory size ofstrings in the wchar string pool
// --------------------------------------------------------------------------------------
BLUEIMPORT void BlueGetSharedStringStatistics(
	size_t& charPoolCount, 
	size_t& charPoolSize, 
	size_t& wcharPoolCount, 
	size_t& wcharPoolSize )
{
	CcpAutoMutex lock( GetPoolMutex() );
	auto& charPool = GetCharPool();
	charPoolCount = charPool.GetStringCount();
	charPoolSize = charPool.GetTableSize();
	auto& wcharPool = GetWCharPool();
	wcharPoolCount = wcharPool.GetStringCount();
	wcharPoolSize = wcharPool.GetTableSize();
}

// --------------------------------------------------------------------------------------
// Description:
//   Wraps a shared string in a BlueScriptValue object to pass to scripting engine.
// Arguments:
//   args - Script call arguments
//   val - shared string
// Return value:
//   string wrapped in a BlueScriptValue object
// --------------------------------------------------------------------------------------
BlueScriptValue BlueWrapReturnValueImpl( 
		BlueScriptArguments args, 
		const BlueSharedString& val )
{
#if BLUE_WITH_PYTHON
	return PyString_FromString( val.c_str() );
#elif BLUE_NO_EXPOSURE
	return nullptr;
#endif
}

// --------------------------------------------------------------------------------------
// Description:
//   Extracts a shared string from a scripting value. The function raises script error
//   if it fails.
// Arguments:
//   argument - Scripting value
//   result - (out) resulting shared string
//   argID - argument index (used for error reporting)
//   isBlueType - unused
// Return value:
//   true if the string was successully extracted
//   false otherwise
// --------------------------------------------------------------------------------------
bool BlueExtractArgumentImpl( 
		BlueScriptValue argument, 
		BlueSharedString& result, 
		unsigned int argID, 
		std::false_type isBlueType )
{
	std::string str;
	bool success = BlueExtractString( argument, str );
	if( !success )
	{
#if BLUE_NO_EXPOSURE
#else
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "string" );
#endif
		return false;
	}
	result = BlueSharedString( str );

	return true;
}

BlueScriptValue BlueWrapReturnValueImpl(
	BlueScriptArguments args,
	const BlueSharedStringW& val )
{
#if BLUE_WITH_PYTHON
	return PyUnicode_FromUnicode( (const Py_UNICODE*)val.c_str(), wcslen( val.c_str() ) );
#elif BLUE_NO_EXPOSURE
	return nullptr;
#endif
}

bool BlueExtractArgumentImpl(
	BlueScriptValue argument,
	BlueSharedStringW& result,
	unsigned int argID,
	std::false_type isBlueType )
{
	std::wstring str;
	bool success = BlueExtractWString( argument, str );
	if( !success )
	{
#if BLUE_NO_EXPOSURE
#else
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "unicode" );
#endif
		return false;
	}
	result = BlueSharedStringW( str );

	return true;
}
