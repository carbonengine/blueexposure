// Copyright © 2026 CCP ehf.

#pragma once

#include "BlueFunctionTraits.h"
#include "BlueVectorTypes.h"


template <typename T>
const char* BlueGetArgumentTypeName();

template <typename T>
const char* BlueGetReturnTypeName();

namespace BlueExposureTypeSignaturePrivate
{

	
template <typename fnType, typename arityType>
struct last_argument_impl
{
};

template <typename fnType>
struct last_argument_impl<fnType, arity1Type>
{
	typedef typename function_traits<fnType>::arg1_type type;
};

template <typename fnType>
struct last_argument_impl<fnType, arity2Type>
{
	typedef typename function_traits<fnType>::arg2_type type;
};

template <typename fnType>
struct last_argument_impl<fnType, arity3Type>
{
	typedef typename function_traits<fnType>::arg3_type type;
};

template <typename fnType>
struct last_argument_impl<fnType, arity4Type>
{
	typedef typename function_traits<fnType>::arg4_type type;
};

template <typename fnType>
struct last_argument_impl<fnType, arity5Type>
{
	typedef typename function_traits<fnType>::arg5_type type;
};

template <typename fnType>
struct last_argument_impl<fnType, arity6Type>
{
	typedef typename function_traits<fnType>::arg6_type type;
};

template <typename fnType>
struct last_argument_impl<fnType, arity7Type>
{
	typedef typename function_traits<fnType>::arg7_type type;
};

template <typename fnType>
struct last_argument_impl<fnType, arity8Type>
{
	typedef typename function_traits<fnType>::arg8_type type;
};

template <typename fnType>
struct last_argument_impl<fnType, arity9Type>
{
	typedef typename function_traits<fnType>::arg9_type type;
};


template <typename fnType>
struct last_argument
{
	typedef typename last_argument_impl<fnType, typename function_traits<fnType>::arity_type>::type type;
};

template <typename T>
struct remove_optional
{
	typedef T type;
};

template <typename T>
struct remove_optional<Be::Optional<T>>
{
	typedef T type;
};

template <typename T, T defaultValue>
struct remove_optional<Be::OptionalWithDefaultValue<T, defaultValue>>
{
	typedef T type;
};


template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity0Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 0;
	signature.optionalCount = 0;
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity1Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 1;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity2Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 2;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.argumentTypes[1] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg2_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity3Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 3;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.argumentTypes[1] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg2_type>();
	signature.argumentTypes[2] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg3_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity4Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 4;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.argumentTypes[1] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg2_type>();
	signature.argumentTypes[2] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg3_type>();
	signature.argumentTypes[3] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg4_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity5Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 5;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.argumentTypes[1] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg2_type>();
	signature.argumentTypes[2] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg3_type>();
	signature.argumentTypes[3] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg4_type>();
	signature.argumentTypes[4] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg5_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity6Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 6;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.argumentTypes[1] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg2_type>();
	signature.argumentTypes[2] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg3_type>();
	signature.argumentTypes[3] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg4_type>();
	signature.argumentTypes[4] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg5_type>();
	signature.argumentTypes[5] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg6_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity7Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 7;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.argumentTypes[1] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg2_type>();
	signature.argumentTypes[2] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg3_type>();
	signature.argumentTypes[3] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg4_type>();
	signature.argumentTypes[4] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg5_type>();
	signature.argumentTypes[5] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg6_type>();
	signature.argumentTypes[6] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg7_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity8Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 8;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.argumentTypes[1] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg2_type>();
	signature.argumentTypes[2] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg3_type>();
	signature.argumentTypes[3] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg4_type>();
	signature.argumentTypes[4] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg5_type>();
	signature.argumentTypes[5] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg6_type>();
	signature.argumentTypes[6] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg7_type>();
	signature.argumentTypes[7] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg8_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}

template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity9Type, std::false_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<typename function_traits<fnType>::return_type>();
	signature.argumentCount = 9;
	signature.argumentTypes[0] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg1_type>();
	signature.argumentTypes[1] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg2_type>();
	signature.argumentTypes[2] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg3_type>();
	signature.argumentTypes[3] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg4_type>();
	signature.argumentTypes[4] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg5_type>();
	signature.argumentTypes[5] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg6_type>();
	signature.argumentTypes[6] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg7_type>();
	signature.argumentTypes[7] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg8_type>();
	signature.argumentTypes[8] = BlueGetArgumentTypeName<typename function_traits<fnType>::arg9_type>();
	signature.optionalCount = uint32_t( function_traits<fnType>::numOptional );
}


template< typename fnType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arity0Type, std::true_type returnsBlueResult )
{
	signature.returnType = BlueGetReturnTypeName<void>();
	signature.argumentCount = 0;
	signature.optionalCount = 0;
}

template< typename fnType, typename arityType >
void BlueGetFunctionSignature( Be::BlueExposureFunctionSignature& signature, fnType method, arityType arity, std::true_type returnsBlueResult )
{
	BlueGetFunctionSignature( signature, method, arity, std::false_type() );

	if( std::is_reference<typename last_argument<fnType>::type>::type::value )
	{
		signature.returnType = signature.argumentTypes[--signature.argumentCount];
	}
	else if( is_pointer_to_pointer_to_blue<typename last_argument<fnType>::type>::type::value )
	{
		signature.returnType = BlueGetReturnTypeName<typename std::remove_pointer<typename last_argument<fnType>::type>::type>();
		--signature.argumentCount;
	}
	else
	{
		signature.returnType = BlueGetReturnTypeName<void>();
	}
}


template <typename T>
struct TypeSignature
{
	static const char* ArgumentType()
	{
		return "Any";
	}
	static const char* ReturnType()
	{
		return "Any";
	}
};

template <>
struct TypeSignature<float>
{
	static const char* ArgumentType()
	{
		return "float";
	}
	static const char* ReturnType()
	{
		return "float";
	}
};

template <>
struct TypeSignature<double>
{
	static const char* ArgumentType()
	{
		return "float";
	}
	static const char* ReturnType()
	{
		return "float";
	}
};

template <>
struct TypeSignature<int32_t>
{
	static const char* ArgumentType()
	{
		return "int";
	}
	static const char* ReturnType()
	{
		return "int";
	}
};

template <>
struct TypeSignature<uint32_t>
{
	static const char* ArgumentType()
	{
		return "int";
	}
	static const char* ReturnType()
	{
		return "int";
	}
};

template <>
struct TypeSignature<uint8_t>
{
	static const char* ArgumentType()
	{
		return "int";
	}
	static const char* ReturnType()
	{
		return "int";
	}
};

template <>
struct TypeSignature<uint16_t>
{
	static const char* ArgumentType()
	{
		return "int";
	}
	static const char* ReturnType()
	{
		return "int";
	}
};

template <>
struct TypeSignature<bool>
{
	static const char* ArgumentType()
	{
		return "bool";
	}
	static const char* ReturnType()
	{
		return "bool";
	}
};

template <>
struct TypeSignature<std::string>
{
	static const char* ArgumentType()
	{
		return "basestring";
	}
	static const char* ReturnType()
	{
		return "str";
	}
};

template <>
struct TypeSignature<const char*>
{
	static const char* ArgumentType()
	{
		return "basestring";
	}
	static const char* ReturnType()
	{
		return "str";
	}
};

template <>
struct TypeSignature<std::wstring>
{
	static const char* ArgumentType()
	{
		return "basestring";
	}
	static const char* ReturnType()
	{
		return "unicode";
	}
};

template <>
struct TypeSignature<const wchar_t*>
{
	static const char* ArgumentType()
	{
		return "basestring";
	}
	static const char* ReturnType()
	{
		return "unicode";
	}
};

template <>
struct TypeSignature<int64_t>
{
	static const char* ArgumentType()
	{
		return "long";
	}
	static const char* ReturnType()
	{
		return "long";
	}
};

template <>
struct TypeSignature<uint64_t>
{
	static const char* ArgumentType()
	{
		return "long";
	}
	static const char* ReturnType()
	{
		return "long";
	}
};

template <>
struct TypeSignature<Vector2>
{
	static const char* ArgumentType()
	{
		return "(float, float)";
	}
	static const char* ReturnType()
	{
		return "(float, float)";
	}
};

template <>
struct TypeSignature<Vector2d>
{
	static const char* ArgumentType()
	{
		return "(float, float)";
	}
	static const char* ReturnType()
	{
		return "(float, float)";
	}
};

template <>
struct TypeSignature<Vector3>
{
	static const char* ArgumentType()
	{
		return "(float, float, float)";
	}
	static const char* ReturnType()
	{
		return "(float, float, float)";
	}
};

template <>
struct TypeSignature<Vector3d>
{
	static const char* ArgumentType()
	{
		return "(float, float, float)";
	}
	static const char* ReturnType()
	{
		return "(float, float, float)";
	}
};

template <>
struct TypeSignature<Vector3i>
{
	static const char* ArgumentType()
	{
		return "(int, int, int)";
	}
	static const char* ReturnType()
	{
		return "(int, int, int)";
	}
};

template <>
struct TypeSignature<Vector4>
{
	static const char* ArgumentType()
	{
		return "(float, float, float, float)";
	}
	static const char* ReturnType()
	{
		return "(float, float, float, float)";
	}
};

template <>
struct TypeSignature<Vector4d>
{
	static const char* ArgumentType()
	{
		return "(float, float, float, float)";
	}
	static const char* ReturnType()
	{
		return "(float, float, float, float)";
	}
};

template <>
struct TypeSignature<Color>
{
	static const char* ArgumentType()
	{
		return "(float, float, float, float)";
	}
	static const char* ReturnType()
	{
		return "(float, float, float, float)";
	}
};

template <>
struct TypeSignature<Quaternion>
{
	static const char* ArgumentType()
	{
		return "(float, float, float, float)";
	}
	static const char* ReturnType()
	{
		return "(float, float, float, float)";
	}
};

template <>
struct TypeSignature<Matrix>
{
	static const char* ArgumentType()
	{
		return "((float, float, float, float), (float, float, float, float), (float, float, float, float), (float, float, float, float))";
	}
	static const char* ReturnType()
	{
		return "((float, float, float, float), (float, float, float, float), (float, float, float, float), (float, float, float, float))";
	}
};

template <typename T>
struct TypeSignature<std::vector<T>>
{
	static const char* ArgumentType()
	{
		static std::string value;
		if( value.empty() )
		{
			value = BlueGetArgumentTypeName<T>();
			value = "list[" + value + "]";
		}
		return value.c_str();
	}
	static const char* ReturnType()
	{
		static std::string value;
		if( value.empty() )
		{
			value = BlueGetReturnTypeName<T>();
			value = "list[" + value + "]";
		}
		return value.c_str();
	}
};

template <typename K, typename V>
struct TypeSignature<std::map<K, V>>
{
	static const char* ArgumentType()
	{
		static std::string result;
		if( result.empty() )
		{
			auto key = BlueGetArgumentTypeName<K>();
			auto value = BlueGetArgumentTypeName<V>();
			result = "dict[";
			result += key;
			result += ", ";
			result += value;
			result += "]";
		}
		return result.c_str();
	}
	static const char* ReturnType()
	{
		static std::string result;
		if( result.empty() )
		{
			auto key = BlueGetReturnTypeName<K>();
			auto value = BlueGetReturnTypeName<V>();
			result = "dict[";
			result += key;
			result += ", ";
			result += value;
			result += "]";
		}
		return result.c_str();
	}
};

template <typename T1, typename T2>
struct TypeSignature<std::pair<T1, T2>>
{
	static const char* ArgumentType()
	{
		static std::string result;
		if( result.empty() )
		{
			auto key = BlueGetArgumentTypeName<T1>();
			auto value = BlueGetArgumentTypeName<T2>();
			result = "(";
			result += key;
			result += ", ";
			result += value;
			result += ")";
		}
		return result.c_str();
	}
	static const char* ReturnType()
	{
		static std::string result;
		if( result.empty() )
		{
			auto key = BlueGetReturnTypeName<T1>();
			auto value = BlueGetReturnTypeName<T2>();
			result = "(";
			result += key;
			result += ", ";
			result += value;
			result += ")";
		}
		return result.c_str();
	}
};

template <typename T>
struct TypeSignature<BluePtr<T>>
{
	static const char* ArgumentType()
	{
		return BlueGetArgumentTypeName<T>();
	}
	static const char* ReturnType()
	{
		return BlueGetReturnTypeName<T>();
	}
};

#ifdef _MSC_VER
template <>
struct TypeSignature<unsigned long>
{
	static const char* ArgumentType()
	{
		return "long";
	}
	static const char* ReturnType()
	{
		return "long";
	}
};
#else
template <>
struct TypeSignature<long>
{
	static const char* ArgumentType()
	{
		return "long";
	}
	static const char* ReturnType()
	{
		return "long";
	}
};
#endif

template <>
struct TypeSignature<void>
{
	static const char* ArgumentType()
	{
		return "None";
	}
	static const char* ReturnType()
	{
		return "None";
	}
};


template <typename T, typename IsBlueObj>
struct TypeSignatureForValueOrBlueObjectImpl
{
};

template <>
struct TypeSignatureForValueOrBlueObjectImpl<IRoot*, std::true_type>
{
	static const char* ArgumentType()
	{
		return "IRoot | None";
	}
	static const char* ReturnType()
	{
		return "IRoot";
	}
};

template <>
struct TypeSignatureForValueOrBlueObjectImpl<const IRoot*, std::true_type>
{
	static const char* ArgumentType()
	{
		return "IRoot | None";
	}
	static const char* ReturnType()
	{
		return "IRoot";
	}
};

template <typename T>
struct TypeSignatureForValueOrBlueObjectImpl<T, std::true_type>
{
	static const char* ArgumentType()
	{
		static std::string result;
		if( result.empty() )
		{
			auto& iid = BlueInterfaceIID<typename remove_const_and_reference<typename std::remove_pointer<T>::type>::type>();
			result = iid.GetName();
			result += "| None";
		}
		return result.c_str();
	}
	static const char* ReturnType()
	{
		auto& iid = BlueInterfaceIID<typename remove_const_and_reference<typename std::remove_pointer<T>::type>::type>();
		return iid.GetName();
	}
};

template <typename T>
struct TypeSignatureForValueOrBlueObjectImpl<T, std::false_type>
{
	static const char* ArgumentType()
	{
		return TypeSignature<T>::ArgumentType();
	}
	static const char* ReturnType()
	{
		return TypeSignature<T>::ReturnType();
	}
};


template <typename T>
struct TypeSignatureForValueOrBlueObject
{
	typedef typename remove_const_and_reference<typename remove_optional<typename remove_const_and_reference<T>::type>::type>::type type;

	static const char* ArgumentType()
	{
		return TypeSignatureForValueOrBlueObjectImpl<type, typename is_blue_type<type>::type>::ArgumentType();
	}
	static const char* ReturnType()
	{
		return TypeSignatureForValueOrBlueObjectImpl<type, typename is_blue_type<type>::type>::ReturnType();
	}
};


}

template <typename T>
const char* BlueGetArgumentTypeName()
{
	return BlueExposureTypeSignaturePrivate::TypeSignatureForValueOrBlueObject<typename remove_const_and_reference<T>::type>::ArgumentType();
}

template <typename T>
const char* BlueGetReturnTypeName()
{
	return BlueExposureTypeSignaturePrivate::TypeSignatureForValueOrBlueObject<typename remove_const_and_reference<T>::type>::ReturnType();
}

template< typename fnType >
Be::BlueExposureFunctionSignature BlueGetFunctionSignature( fnType method, uint32_t numOptional = 0 )
{
	Be::BlueExposureFunctionSignature result;
	BlueExposureTypeSignaturePrivate::BlueGetFunctionSignature( 
		result, 
		method, 
		typename function_traits<fnType>::arity_type(), 
		typename is_be_result<typename function_traits<fnType>::return_type>::type() );
	result.optionalCount = numOptional;
	return result;
}