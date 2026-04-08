// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueTypeTraits_h
#define BlueTypeTraits_h

#include <type_traits>
#include "BlueClass.h"
#include "BlueSharedString.h"
#include "BlueWeakRef.h"

struct IRoot;
struct Vector2;
struct Vector2d;
struct Vector3;
struct Vector3d;
struct Vector3i;
struct Vector4;
struct Vector4d;
struct Matrix;
struct Quaternion;
struct Color;
class BlueScriptCallback;

// Automatic Be::VARTYPE extraction by passing in a C++ type, use like this:
// Be::VARTYPE t = TypeTraits<long>::VARTYPE_VALUE;
// Note that instead of 'long' you can use most basic C++ types!

template<typename T> class BlueTypeTraits
{
public:
	// Note that we don't want to define a VARTYPE_VALUE here,
	// it is defined by the specializations below. Leaving
	// the non-specialized template without a VARTYPE_VALUE
	// means we get a compilation error on unsupported types.
};

#define BLUE_DEFINE_TYPE_TRAIT( cppType_, varType_ ) \
	template<> struct BlueTypeTraits< cppType_ > \
{ \
	static const Be::VARTYPE VARTYPE_VALUE = varType_; \
}

BLUE_DEFINE_TYPE_TRAIT( int64_t, Be::INT64 );
BLUE_DEFINE_TYPE_TRAIT( uint64_t, Be::INT64 );
#if defined(_MSC_VER) || defined(__ANDROID__)
BLUE_DEFINE_TYPE_TRAIT( long, Be::LONG );
BLUE_DEFINE_TYPE_TRAIT( unsigned long, Be::LONG );
#endif
BLUE_DEFINE_TYPE_TRAIT( int32_t, Be::LONG );
BLUE_DEFINE_TYPE_TRAIT( uint32_t, Be::LONG );
BLUE_DEFINE_TYPE_TRAIT( short, Be::SHORT );
BLUE_DEFINE_TYPE_TRAIT( unsigned short, Be::SHORT );
BLUE_DEFINE_TYPE_TRAIT( float, Be::FLOAT );
BLUE_DEFINE_TYPE_TRAIT( double, Be::DOUBLE );
BLUE_DEFINE_TYPE_TRAIT( bool, Be::BOOL );
BLUE_DEFINE_TYPE_TRAIT( char, Be::BYTE );
BLUE_DEFINE_TYPE_TRAIT( unsigned char, Be::BYTE );
BLUE_DEFINE_TYPE_TRAIT( Vector2, Be::FLOATARRAY );
BLUE_DEFINE_TYPE_TRAIT( Vector2d, Be::DOUBLEARRAY );
BLUE_DEFINE_TYPE_TRAIT( Vector3, Be::FLOATARRAY );
BLUE_DEFINE_TYPE_TRAIT( Vector3d, Be::DOUBLEARRAY );
BLUE_DEFINE_TYPE_TRAIT( Vector3i, Be::INTARRAY );
BLUE_DEFINE_TYPE_TRAIT( Vector4, Be::FLOATARRAY );
BLUE_DEFINE_TYPE_TRAIT( Vector4d, Be::DOUBLEARRAY );
BLUE_DEFINE_TYPE_TRAIT( Matrix, Be::FLOATARRAY );
BLUE_DEFINE_TYPE_TRAIT( Color, Be::FLOATARRAY );
BLUE_DEFINE_TYPE_TRAIT( Quaternion, Be::FLOATARRAY );
#if BLUE_WITH_PYTHON
BLUE_DEFINE_TYPE_TRAIT( PyObject*, Be::PYOBJECTPTR );
#endif
BLUE_DEFINE_TYPE_TRAIT( char*, Be::CSTRING );
BLUE_DEFINE_TYPE_TRAIT( wchar_t*, Be::WCSTRING );
BLUE_DEFINE_TYPE_TRAIT( std::string, Be::STDSTRING );
BLUE_DEFINE_TYPE_TRAIT( std::wstring, Be::STDWSTRING );
#ifdef __APPLE__
// clang on mac treats size_t as a totally separate type
BLUE_DEFINE_TYPE_TRAIT( size_t, sizeof( size_t ) == 32 ? Be::LONG : Be::INT64 );
BLUE_DEFINE_TYPE_TRAIT( ssize_t, sizeof( ssize_t ) == 32 ? Be::LONG : Be::INT64 );
#endif

#ifdef _WIN32
BLUE_DEFINE_TYPE_TRAIT( HWND, Be::LONG );
#endif

BLUE_DEFINE_TYPE_TRAIT( BlueScriptCallback, Be::SCRIPTCALLBACK );
BLUE_DEFINE_TYPE_TRAIT( BlueSharedString, Be::SHAREDSTRING );
BLUE_DEFINE_TYPE_TRAIT( BlueSharedStringW, Be::SHAREDSTRINGW );

extern BLUEIMPORT const Be::IID BlueDefaultIID;
extern BLUEIMPORT const Be::IID BlueRotationIID;
extern BLUEIMPORT const Be::IID BlueDirectionIID;
extern BLUEIMPORT const Be::IID BlueColorIID;
extern BLUEIMPORT const Be::IID BlueMatrixIID;
extern BLUEIMPORT const Be::IID BlueTimeIID;

BLUE_DECLARE_INTERFACE_EXPORT( IList );
BLUE_DECLARE_INTERFACE_EXPORT( IBlueDict );
BLUE_DECLARE_INTERFACE_EXPORT( IBlueStructureList );

//////////////////////////////////////////////////////////////////////////
// Useful Type-traits
//////////////////////////////////////////////////////////////////////////

template< typename T >
struct is_be_result
{
	typedef std::false_type type;
};

template<typename T>
struct is_be_result< Be::Result<T> >
{
	typedef std::true_type type;
};

template< typename T >
struct remove_const_and_reference
{
	typedef
		typename std::remove_const<
			typename std::remove_reference< T >::type
		>::type
		type;
};

template< typename T >
struct is_blue_type
{
	typedef
		typename std::is_base_of<
			IRoot,
			typename std::remove_pointer< T >::type
		>::type
		type;
};

template< typename T >
struct is_blue_type<Be::Optional<T> >
{
	typedef typename is_blue_type<T>::type type;
};

template< typename T >
struct is_pointer_to_blue
{
	typedef
		typename std::integral_constant<
			bool,
			std::is_base_of<
				IRoot, 
				typename std::remove_pointer< T >::type
			>::value 
			&& std::is_pointer<T>::value
		>::type
		type;
};

template< typename T >
struct is_pointer_to_pointer_to_blue
{
	typedef
		typename std::integral_constant<
			bool,
			std::is_base_of<
				IRoot, 
				typename std::remove_pointer< 
					typename std::remove_pointer< T >::type>::type
			>::value
			&& std::is_pointer<typename std::remove_pointer< T >::type>::value
		>::type
		type;
};

template< typename T >
struct is_char_array
{
	typedef
		typename std::integral_constant<
			bool,
			std::is_array<T>::value 
			&& std::is_same<
				typename std::remove_extent<
					typename remove_const_and_reference<T>::type
				>::type, 
				char
			>::value
		>::type
		type;
};

//////////////////////////////////////////////////////////////////////////
// Blue Type Trait Functions
//////////////////////////////////////////////////////////////////////////



template<typename T>
struct VarTypeForVariable
{
private:
	template <typename T2, typename IsPointerToBlue>
	struct CheckPointerToBlue
	{
	};

	template <typename T2>
	struct CheckPointerToBlue<T2, std::false_type>
	{
		// If you get a cryptic template compilation error that points to the
		// line below, it is probably because you are using MAP_ATTRIBUTE on
		// an unsupported type. This could be the result of exposing a variable
		// of a type that has only been forward-declared and isn't fully
		// known. In particular, Blue objects and interfaces must be fully
		// declared (include the header file) so the compiler can figure out
		// that they inherit from IRoot.
		static const Be::VARTYPE type = BlueTypeTraits<T2>::VARTYPE_VALUE;
	};

	template <typename T2>
	struct CheckPointerToBlue<T2, std::true_type>
	{
		static const Be::VARTYPE type = Be::IROOTPTR;
	};

	template <typename T2, typename IsCharacterArray>
	struct CheckCharacterArray
	{
	};

	template <typename T2>
	struct CheckCharacterArray<T2, std::false_type>
	{
		static const Be::VARTYPE type = CheckPointerToBlue<T2, typename is_pointer_to_blue<T2>::type>::type;
	};

	template <typename T2>
	struct CheckCharacterArray<T2, std::true_type>
	{
		static const Be::VARTYPE type = Be::CHARARRAY;
	};


	template <typename T2, typename IsEnum>
	struct GetVarTypeForVariableImpl
	{
	};

	template <typename T2>
	struct GetVarTypeForVariableImpl<T2, std::true_type>
	{
		static const Be::VARTYPE type = Be::LONG;
	};

	template <typename T2>
	struct GetVarTypeForVariableImpl<T2, std::false_type>
	{
		static const Be::VARTYPE type = CheckCharacterArray<T2, typename is_char_array<T2>::type>::type;
	};

	template<typename T2>
	struct GetVarTypeForVariableImpl<RootParentLock<T2>, std::false_type>
	{
		static const Be::VARTYPE type = Be::IROOT;
	};

	template<typename T2>
	struct GetVarTypeForVariableImpl<RootParentLockWR<T2>, std::false_type>
	{
		static const Be::VARTYPE type = Be::IROOT;
	};

	template<typename T2>
	struct GetVarTypeForVariableImpl<RootRefLock<T2>, std::false_type>
	{
		static const Be::VARTYPE type = Be::IROOT;
	};

	template<typename T2>
	struct GetVarTypeForVariableImpl<RootNoLock<T2>, std::false_type>
	{
		static const Be::VARTYPE type = Be::IROOT;
	};

	template<typename T2>
	struct GetVarTypeForVariableImpl<RootNoLockWR<T2>, std::false_type>
	{
		static const Be::VARTYPE type = Be::IROOT;
	};

	template<typename T2>
	struct GetVarTypeForVariableImpl<BlueBasicPtr<T2>, std::false_type>
	{
		static const Be::VARTYPE type = Be::IROOTPTR;
	};

	template<typename T2>
	struct GetVarTypeForVariableImpl<BluePtr<T2>, std::false_type>
	{
		static const Be::VARTYPE type = Be::IROOTPTR;
	};

	template<typename T2>
	struct GetVarTypeForVariableImpl<BlueWeakRef<T2>, std::false_type>
	{
		static const Be::VARTYPE type = Be::IROOTWEAKREF;
	};


	template <typename T2>
	struct CleanType
	{
		typedef typename std::remove_const<typename std::remove_reference<T2>::type>::type type;
	};

public:
	static const Be::VARTYPE type = GetVarTypeForVariableImpl<typename CleanType<T>::type, 
		typename std::is_enum<typename CleanType<T>::type>::type>::type;
};


namespace BlueListUtils
{
class BlueListBase;
}

template <typename T>
struct remove_lock
{
	typedef T type;
};

template <typename T>
struct remove_lock<RootParentLock<T>>
{
	typedef T type;
};

template <typename T>
struct remove_lock<RootParentLockWR<T>>
{
	typedef T type;
};

template <typename T>
struct remove_lock<RootRefLock<T>>
{
	typedef T type;
};

template <typename T>
struct remove_lock<RootNoLock<T>>
{
	typedef T type;
};

template <typename T>
struct remove_lock<RootNoLockWR<T>>
{
	typedef T type;
};



template<typename T> inline const Be::IID* GetBlueIIDBluePointerHelper( std::false_type isBlueList, std::false_type isBlueDict, std::false_type isStructureList )
{
	return &BlueInterfaceIID<T>();
}

template<typename T> inline const Be::IID* GetBlueIIDBluePointerHelper( std::true_type isBlueList, std::false_type isBlueDict, std::false_type isStructureList )
{
	return &BlueInterfaceIID<IList>();
}

template<typename T> inline const Be::IID* GetBlueIIDBluePointerHelper( std::false_type isBlueList, std::true_type isBlueDict, std::false_type isStructureList )
{
	return &BlueInterfaceIID<IBlueDict>();
}

template<typename T> inline const Be::IID* GetBlueIIDBluePointerHelper( std::false_type isBlueList, std::false_type isBlueDict, std::true_type isStructureList )
{
	return &BlueInterfaceIID<IBlueStructureList>();
}

template<typename T> inline const Be::IID* GetBlueIIDHelper( std::true_type isRawBluePointer )
{
	typedef typename remove_lock<typename std::remove_const<typename std::remove_pointer<T>::type>::type>::type RawType;

	return GetBlueIIDBluePointerHelper<RawType>( typename std::is_base_of<IList, RawType>::type(), typename std::is_base_of<IBlueDict, RawType>::type(), typename std::is_base_of<IBlueStructureList, RawType>::type() );
}

template<typename T> inline const Be::IID* GetBlueIIDHelper( std::false_type isRawBluePointer )
{
	return nullptr;
}

template<typename T> inline const Be::IID* GetBlueIIDPointerHelper( std::true_type isPointer )
{
	return GetBlueIIDHelper<T>( typename is_pointer_to_blue<T>::type() );
}

template<typename T> inline const Be::IID* GetBlueIIDPointerHelper( std::false_type isPointer )
{
	return GetBlueIIDHelper<T*>( typename is_pointer_to_blue<T*>::type() );
}


template <typename T>
struct GetBlueIIDImpl
{
	static const Be::IID* GetIID()
	{
		return GetBlueIIDPointerHelper<T>( typename std::is_pointer<T>::type() );
	}
};

template <typename T>
struct GetBlueIIDImpl<BluePtr<T>>
{
	static const Be::IID* GetIID()
	{
		return &BlueInterfaceIID<T>();
	}
};

template <>
struct GetBlueIIDImpl<Quaternion>
{
	static const Be::IID* GetIID()
	{
		return &BlueRotationIID;
	}
};

template <>
struct GetBlueIIDImpl<Color>
{
	static const Be::IID* GetIID()
	{
		return &BlueColorIID;
	}
};

template <>
struct GetBlueIIDImpl<Matrix>
{
	static const Be::IID* GetIID()
	{
		return &BlueMatrixIID;
	}
};



// Most types don't have a Blue interfaceID
template<typename T> inline const Be::IID* GetBlueIID()
{
	typedef typename std::remove_const<typename std::remove_reference<T>::type>::type Stripped;
	return GetBlueIIDImpl<Stripped>::GetIID();
}

#endif // BlueTypeTraits_h