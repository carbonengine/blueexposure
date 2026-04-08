// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestReferences.h"

BLUE_DEFINE( TestReferences );

const Be::ClassInfo* TestReferences::ExposeToBlue()
{
	EXPOSURE_BEGIN( TestReferences, "" )
		MAP_ATTRIBUTE
		(
			"weakRef",
			m_weakRef,
			"",
			Be::READWRITE | Be::PERSIST
		)
		MAP_ATTRIBUTE
		(
			"strongRef",
			m_strongRef,
			"",
			Be::READWRITE | Be::PERSIST
		)
	EXPOSURE_END()
}
