// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueWrapReturnValue_h
#define BlueWrapReturnValue_h

#include <StringConversions.h>

#if BLUE_WITH_PYTHON
#include "BlueWrapReturnValuePython.h"
#endif

template< typename valueType>
BlueScriptValue BlueWrapReturnValueHelper( BlueScriptArguments args, valueType val, std::false_type isPointerToBlue )
{
	return BlueWrapReturnValueImpl( args, val );
}

template< typename valueType >
BlueScriptValue BlueWrapReturnValue( BlueScriptArguments args, valueType val )
{
	return BlueWrapReturnValueHelper( args, val, typename is_pointer_to_blue<valueType>::type() );
}

#endif // BlueWrapReturnValue_h
