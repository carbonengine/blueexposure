// Copyright © 2026 CCP ehf.

#pragma once
#ifndef StringBeResult_h
#define StringBeResult_h

#include "BlueStdResult.h"

template<>
struct Be::Result<std::string>
{
	Result<std::string>() {}
	Result<std::string>( const std::string& s ) : value( s ) {}
	Result<std::string>( const Result& other ) : value( other.value ) {}
	std::string value;
};

template <>
inline bool BeIsSuccess<std::string>( const Be::Result<std::string>& result )
{
	return result.value.empty();
}

template <>
inline const char* BeGetErrorMessage<std::string>( const Be::Result<std::string>& result )
{
	return result.value.c_str();
}

BLUE_BEGIN_GET_EXCEPTION_INLINE( Be::Result<std::string> )
	return BLUE_GET_EXCEPTION( BlueStdRuntimeError );
BLUE_END_GET_EXCEPTION()

#endif // StringBeResult_h