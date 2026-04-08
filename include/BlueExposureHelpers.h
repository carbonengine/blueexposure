// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueExposureHelpers_h
#define BlueExposureHelpers_h

#include "BlueFunctionTraits.h"
#include "BlueExtractArgument.h"
#include "BlueExposureTypeSignature.h"
#include "BlueExposureMacros.h"

#if BLUE_WITH_PYTHON
#include "BlueExposureHelpersPython.h"
#elif BLUE_NO_EXPOSURE
template< typename FnType, FnType fn >
void BlueFunctionHelper()
{
}
template< typename class_type, typename memFnType, memFnType memFn >
void BlueMethodHelper()
{
}

#endif

// Get the VARTYPE for the return value of the property getter function.
// This is a specialization for the case where the return type is a Be::Result,
// meaning we use the type of the first argument - expected to be a reference.
template< typename Class, typename FunctionType, FunctionType getter>
Be::VARTYPE BlueGetPropertyTypeBeResult( std::true_type isBeResult )
{
	typedef 
		typename remove_const_and_reference<
			typename function_traits<FunctionType>::arg1_type
		>::type
		PropertyType;

	return VarTypeForVariable<PropertyType>::type;
}

// Get the VARTYPE for the return value of the property getter function.
// This is a specialization for the case where the return type is not a Be::Result,
// meaning we use the return type of the function.
template< typename Class, typename FunctionType, FunctionType getter>
Be::VARTYPE BlueGetPropertyTypeBeResult( std::false_type isBeResult )
{
	typedef 
		typename function_traits<FunctionType>::return_type 
		PropertyType;

	typedef
		typename remove_const_and_reference<PropertyType>::type
		StrippedPropertyType;

	return VarTypeForVariable<StrippedPropertyType>::type;
}

// Get the VARTYPE for the return value of the property getter function.
// Note that a Be::Result return type implies that we look at the type
// of the first argument and use that instead.
template< typename Class, typename FunctionType, FunctionType getter>
Be::VARTYPE BlueGetPropertyType()
{
	typedef
		typename function_traits<FunctionType>::return_type
		return_type;

	return BlueGetPropertyTypeBeResult<Class, FunctionType, getter>( typename is_be_result<return_type>::type() );
}


#endif // BlueExposureHelpers_h