// Copyright © 2026 CCP ehf.

#include "include/BlueTypes.h"
#include "include/BlueStringTable.h"
#include <string>

namespace
{
	typedef BlueStringTable<char> StringTable;

	CcpMutex& GetStringTableMutex()
	{
		static CcpMutex s_mutex( "BlueTypes", "GetStringTableMutex" );
		return s_mutex;
	}

	StringTable& GetStringTable()
	{
		static StringTable s_stringTable;
		return s_stringTable;
	}
}

//--------------------------------------------------------------------
// Be ClassId constructor
//--------------------------------------------------------------------
Be::Clsid::Clsid()
{
	m_module = "";
	m_name = "";
	m_hash = 0;
}

Be::Clsid::Clsid( const char* modulename, const char* classname )
	:m_hash( 0 )
{
	CcpAutoMutex lock( GetStringTableMutex() );

	m_module = GetStringTable().GetString( modulename );
	m_name = GetStringTable().GetString( classname, m_hash );
}

//--------------------------------------------------------------------
// Be ClassId methods
//--------------------------------------------------------------------

bool Be::Clsid::InitFromString( const char* string )
{
	// Reset in case the function errors out
	m_module = "";
	m_name = "";
	m_hash = 0;

	const char* firstdot = strchr( string, '.' );

	if( !firstdot )
	{
		CCP_LOGERR( "Cannot convert \"%s\" to a valid class id", string );
		return false;
	}

	CcpMallocBuffer buffer( "buffer", strlen( string ) + 1 );
	memcpy( buffer.get(), string, firstdot - string);
	char* module = buffer.get();
	module[firstdot - string] = '\0';

	CcpAutoMutex lock( GetStringTableMutex() );

	m_module = GetStringTable().GetString( module );
	m_name = GetStringTable().GetString( firstdot + 1, m_hash );

	return true;
}

bool Be::Clsid::IsEqual(const Clsid& other) const
{
	return m_hash == other.m_hash;
}


bool Be::Clsid::operator < (const Clsid &other) const
{
	return m_hash < other.m_hash;
}


Be::Clsid::operator bool() const
{
	return m_hash != 0;
}


const char* Be::Clsid::GetModule() const
{
	return m_module;
}

const char* Be::Clsid::GetName() const
{
	return m_name;
}

unsigned int Be::Clsid::GetHash() const
{
	return m_hash;
}


//--------------------------------------------------------------------
// Be IID methods
//--------------------------------------------------------------------

Be::IID::IID( const char* name ) : m_hash( 0 )
{
	CcpAutoMutex lock( GetStringTableMutex() );

	m_name = GetStringTable().GetString( name, m_hash );
}


bool Be::IID::InitFromString(const char* string)
{
	CcpAutoMutex lock( GetStringTableMutex() );

	m_name = GetStringTable().GetString( string, m_hash );

	return true;
}

bool Be::IID::IsEqual(const IID& other) const
{
	return m_hash == other.m_hash;
}


bool Be::IID::operator <(const IID &other) const
{
	return m_hash < other.m_hash;
}


Be::IID::operator bool() const
{
	return m_hash != 0;
}

const char* Be::IID::GetName() const
{
	return m_name;
}

unsigned int Be::IID::GetHash() const
{
	return m_hash;
}