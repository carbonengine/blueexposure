// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BluePooledString_H
#define BluePooledString_H

#include "BlueTypes.h"
#include <CcpMacros.h>
#include <string>

// --------------------------------------------------------------------------------------
// Description:
//   Template shared string class. Uses shared memory pool for strings, i.e. identical
//   strings will point to the same memory location. Can be used instead of std::string
//   for object properties that don't change too often.
// See Also:
//   BlueSharedString
// --------------------------------------------------------------------------------------
template <typename CharType>
class BlueSharedStringT
{
public:
	BlueSharedStringT();
	BlueSharedStringT( const BlueSharedStringT& other );
	explicit BlueSharedStringT( const char* string );
	explicit BlueSharedStringT( const std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType>>& string );

	const CharType* c_str() const;
	bool empty() const;

	bool operator==( const BlueSharedStringT& other ) const;

	// Note that this operator is only to allow BlueSharedString in maps.
	// It doesn't do a proper string comparison, only compares pointer values.
	bool operator<( const BlueSharedStringT& other ) const;
private:
	union
	{
		const CharType* m_string;
		uint64_t _padding;
	};
};

typedef BlueSharedStringT<char> BlueSharedString;
typedef BlueSharedStringT<wchar_t> BlueSharedStringW;

BLUEIMPORT const char* BlueAllocateSharedStringFromPool( const char* string );
BLUEIMPORT const wchar_t* BlueAllocateSharedStringFromPool( const wchar_t* string );

BLUEIMPORT void BlueGetSharedStringStatistics(
	size_t& charPoolCount, 
	size_t& charPoolSize, 
	size_t& wcharPoolCount, 
	size_t& wcharPoolSize );

BLUEIMPORT BlueScriptValue BlueWrapReturnValueImpl( 
		BlueScriptArguments args, 
		const BlueSharedString& val );
BLUEIMPORT bool BlueExtractArgumentImpl( 
		BlueScriptValue argument, 
		BlueSharedString& result, 
		unsigned int argID, 
		std::false_type isBlueType );

BLUEIMPORT BlueScriptValue BlueWrapReturnValueImpl(
	BlueScriptArguments args,
	const BlueSharedStringW& val );
BLUEIMPORT bool BlueExtractArgumentImpl(
	BlueScriptValue argument,
	BlueSharedStringW& result,
	unsigned int argID,
	std::false_type isBlueType );

template <typename T>
struct BlueSharedStringDefaultValue
{
};

template <>
struct BlueSharedStringDefaultValue<char>
{
	static constexpr const char* VALUE = "";
};

template <>
struct BlueSharedStringDefaultValue<wchar_t>
{
	static constexpr const wchar_t* VALUE = L"";
};


template <typename CharType>
BlueSharedStringT<CharType>::BlueSharedStringT() :
	m_string( BlueAllocateSharedStringFromPool( BlueSharedStringDefaultValue<CharType>::VALUE ) )
{
}

template <typename CharType>
BlueSharedStringT<CharType>::BlueSharedStringT( const BlueSharedStringT& other )
	:m_string( other.m_string )
{
}

template <typename CharType>
BlueSharedStringT<CharType>::BlueSharedStringT( const char* string )
	:m_string( BlueAllocateSharedStringFromPool( string ) )
{
}

template <typename CharType>
BlueSharedStringT<CharType>::BlueSharedStringT( const std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType>>& string )
	:m_string( BlueAllocateSharedStringFromPool( string.c_str() ) )
{
}

template <typename CharType>
const CharType* BlueSharedStringT<CharType>::c_str() const
{
	return m_string;
}

template <typename CharType>
bool BlueSharedStringT<CharType>::empty() const
{
	return !m_string || m_string[0] == 0;
}

template <typename CharType>
bool BlueSharedStringT<CharType>::operator==( const BlueSharedStringT& other ) const
{
	return m_string == other.m_string;
}

template <typename CharType>
bool BlueSharedStringT<CharType>::operator<( const BlueSharedStringT& other ) const
{
	return m_string < other.m_string;
}
#endif
