// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestMultipleInterfaces.h"

BLUE_DEFINE_INTERFACE( ITestInterfaceOne );
BLUE_DEFINE_INTERFACE( ITestInterfaceTwo );

BLUE_DEFINE( TestMultipleInterfaces );

const Be::ClassInfo* TestMultipleInterfaces::ExposeToBlue()
{
	EXPOSURE_BEGIN( TestMultipleInterfaces, "" )
		MAP_INTERFACE( ITestInterfaceOne )
		MAP_INTERFACE( ITestInterfaceTwo )
	EXPOSURE_END()
}
