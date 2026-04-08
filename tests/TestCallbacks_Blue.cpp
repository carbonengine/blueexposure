// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestCallbacks.h"

BLUE_DEFINE( TestCallbacks );

const Be::ClassInfo* TestCallbacks::ExposeToBlue()
{
	EXPOSURE_BEGIN( TestCallbacks, "TestCallbacks is used to help test Blue exposure" )
		MAP_INTERFACE( TestCallbacks )

		MAP_METHOD_AND_WRAP( "GetCallback", GetCallback, "" )
		MAP_METHOD_AND_WRAP( "SetCallback", SetCallback, "" )
		MAP_METHOD_AND_WRAP( "HasValidCallback", HasValidCallback, "" )
		MAP_METHOD_AND_WRAP( "CallCallbackVoid", CallCallbackVoid, "" )
		MAP_METHOD_AND_WRAP( "CallCallbackInt", CallCallbackInt, "" )
		MAP_METHOD_AND_WRAP( "CallCallbackStringInt", CallCallbackStringInt, "" )
		MAP_METHOD_AND_WRAP( "CallCallbackIntStringFloat", CallCallbackIntStringFloat, "" )

		MAP_PROPERTY( "callback", GetCallback, SetCallback, "" )
		MAP_ATTRIBUTE( "callbackAttribute", m_callback, "", Be::READWRITE )
	EXPOSURE_END()
}