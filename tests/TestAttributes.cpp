// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestAttributes.h"

TestAttributes::TestAttributes( IRoot* lockobj ) :
	m_myBool( false ),
	m_myInt( 0 ),
	m_myFloat( 0 ),
	m_myDouble( 0 ),
	m_myInt64( 0 ),
	PARENTLOCK( m_myVector )
#if BLUE_WITH_PYTHON
	, PARENTLOCK( m_myDict )
#endif

{
}
