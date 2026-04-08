// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueStringTable_H
#define BlueStringTable_H

#include <CcpCore.h>

// --------------------------------------------------------------------------------------
// Description:
//   Table for shared strings. New strings are copied to the table if the they 
//   differ from any other string in the table. There is no way to remove strings from 
//   the table.
// --------------------------------------------------------------------------------------
template <typename CharType>
class BlueStringTable
{
public:
	typedef unsigned HashType;

	BlueStringTable()
		:m_strings( "BlueStringTable::m_strings" ),
		m_tableSize( 0 )
	{
	}

	// ----------------------------------------------------------------------------------
	// Description:
	//   Returns a string in the pool which contents is equal to the passed string. 
	//   Either returns an existing string or allocates a new one.
	// Arguments:
	//   string - zero terminated string
	//   hash - (out) hash value of the given string
	// Return value:
	//   string in the memory pool which contents is equal to the passed string
	// ----------------------------------------------------------------------------------
	const CharType* GetString( const CharType* string, HashType& hash )
	{
		size_t size = GetStringSize( string );
		hash = CcpHashFNV1( string, size );
		auto found = m_strings.find( hash );
		if( found != m_strings.end() )
		{
			return found->second;
		}
		CharType* str = CCP_NEW( "Shared string" ) CharType[size];
		if( str )
		{
			memcpy( str, string, size );
			m_tableSize += size;
			m_strings.insert( std::make_pair( hash, str ) );
		}
		return str;
	}

	// ----------------------------------------------------------------------------------
	// Description:
	//   Returns a string in the pool which contents is equal to the passed string. 
	//   Either returns an existing string or allocates a new one.
	// Arguments:
	//   string - zero terminated string
	// Return value:
	//   string in the memory pool which contents is equal to the passed string
	// ----------------------------------------------------------------------------------
	const CharType* GetString( const CharType* string )
	{
		HashType hash;
		return GetString( string, hash );
	}

	// ----------------------------------------------------------------------------------
	// Description:
	//   Returns number of strings in the table.
	// Return value:
	//   number of strings in the table
	// ----------------------------------------------------------------------------------
	size_t GetStringCount() const
	{
		return m_strings.size();
	}

	// ----------------------------------------------------------------------------------
	// Description:
	//   Returns memory occupied by all strings in the table.
	// Return value:
	//   memory occupied by all strings in the table
	// ----------------------------------------------------------------------------------
	size_t GetTableSize() const
	{
		return m_tableSize;
	}
private:
	// --------------------------------------------------------------------------------------
	// Description:
	//   Helper function that returns string size (including the trailing 0) in bytes.
	// Arguments:
	//   string - zero terminated string
	// Return value:
	//   size of the string in bytes
	// --------------------------------------------------------------------------------------
	static size_t GetStringSize( const char* string )
	{
		return strlen( string ) + 1;
	}

	// --------------------------------------------------------------------------------------
	// Description:
	//   Helper function that returns wide string size (including the trailing 0) in bytes.
	// Arguments:
	//   string - zero terminated string
	// Return value:
	//   size of the string in bytes
	// --------------------------------------------------------------------------------------
	static size_t GetStringSize( const wchar_t* string )
	{
		return ( wcslen( string ) + 1 ) * sizeof( wchar_t );
	}

	TrackableStdUnorderedMap<HashType, CharType*> m_strings;
	size_t m_tableSize;
};

#endif
