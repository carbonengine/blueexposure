// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueStdResult_H
#define BlueStdResult_H

BLUE_DECLARE_EXCEPTION_EX( BlueStdIndexError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdKeyError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdAssertionError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdAttributeError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdEofError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdIOError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdMemoryError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdOSError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdOverflowError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdRuntimeError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdTypeError, extern BLUEIMPORT );
BLUE_DECLARE_EXCEPTION_EX( BlueStdValueError, extern BLUEIMPORT );

enum BlueStdResultType
{
	BLUE_STD_RESULT_OK,
	BLUE_STD_RESULT_INDEX_ERROR,
	BLUE_STD_RESULT_KEY_ERROR,
	BLUE_STD_RESULT_ASSERTION_ERROR,
	BLUE_STD_RESULT_ATTRIBUTE_ERROR,
	BLUE_STD_RESULT_EOF_ERROR,
	BLUE_STD_RESULT_IO_ERROR,
	BLUE_STD_RESULT_MEMORY_ERROR,
	BLUE_STD_RESULT_OS_ERROR,
	BLUE_STD_RESULT_OVERFLOW_ERROR,
	BLUE_STD_RESULT_RUNTIME_ERROR,
	BLUE_STD_RESULT_TYPE_ERROR,
	BLUE_STD_RESULT_VALUE_ERROR,
};

namespace Be
{

template <>
struct Result<BlueStdResultType>
{
	Result()
		:m_type( BLUE_STD_RESULT_OK )
	{
	}

	Result( BlueStdResultType type )
		:m_type( type )
	{
	}

	Result( BlueStdResultType type, const char* message )
		:m_type( type ),
		m_message( message )
	{
	}

	BlueStdResultType GetType() const
	{
		return m_type;
	}

	const char* GetMessage() const
	{
		return m_message.c_str();
	}

	operator bool() const
	{
		return m_type == BLUE_STD_RESULT_OK;
	}
private:
	BlueStdResultType m_type;
	std::string m_message;
};

}

typedef Be::Result<BlueStdResultType> BlueStdResult;

template<> inline bool BeIsSuccess( const BlueStdResult& result )
{
	return result;
}

inline const char* BeGetErrorMessage( const BlueStdResult& result )
{
	return result.GetMessage();
}


BLUE_BEGIN_GET_EXCEPTION_INLINE( BlueStdResult )
	switch( result.GetType() )
	{
	case BLUE_STD_RESULT_INDEX_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdIndexError );
	case BLUE_STD_RESULT_KEY_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdKeyError );
	case BLUE_STD_RESULT_ASSERTION_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdAssertionError );
	case BLUE_STD_RESULT_ATTRIBUTE_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdAttributeError );
	case BLUE_STD_RESULT_EOF_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdEofError );
	case BLUE_STD_RESULT_IO_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdIOError );
	case BLUE_STD_RESULT_MEMORY_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdMemoryError );
	case BLUE_STD_RESULT_OS_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdOSError );
	case BLUE_STD_RESULT_OVERFLOW_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdOverflowError );
	case BLUE_STD_RESULT_RUNTIME_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdRuntimeError );
	case BLUE_STD_RESULT_TYPE_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdTypeError );
	case BLUE_STD_RESULT_VALUE_ERROR:
		return BLUE_GET_EXCEPTION( BlueStdValueError );
	default:
		return BLUE_GET_EXCEPTION( BlueStdValueError );
	}
BLUE_END_GET_EXCEPTION()


template <typename T1, typename T2>
struct BeResultChoice: public std::pair<T1, T2>
{
	BeResultChoice()
	{
	}

	BeResultChoice( const T1& result1 )
	{
		this->first = result1;
	}
	BeResultChoice( const T2& result2 )
	{
		this->second = result2;
	}
};

template <typename T1, typename T2>
struct Be::Result<BeResultChoice<T1, T2>>
{
	Result<BeResultChoice<T1, T2>>()
	{
	}

	Result<BeResultChoice<T1, T2>>( const T1& result1 )
		:m_results( result1 )
	{
	}

	Result<BeResultChoice<T1, T2>>( const T2& result2 )
		:m_results( result2 )
	{
	}

	BeResultChoice<T1, T2> m_results;
};

template <typename T1, typename T2> inline bool BeIsSuccess( const Be::Result<BeResultChoice<T1, T2>>& result )
{
	return BeIsSuccess( result.m_results.first ) && BeIsSuccess( result.m_results.second );
}

template <typename T1, typename T2>
inline const char* BeGetErrorMessage( const Be::Result<BeResultChoice<T1, T2>>& result )
{
	if( !BeIsSuccess( result.m_results.first ) )
	{
		return BeGetErrorMessage( result.m_results.first );
	}
	return BeGetErrorMessage( result.m_results.second );
}

#if BLUE_WITH_PYTHON
template <typename T1, typename T2> inline PyObject* BeGetException( const Be::Result<BeResultChoice<T1, T2>>& result ) 
{
	if( !BeIsSuccess( result.m_results.first ) )
	{
		return BeGetException( result.m_results.first );
	}
	return BeGetException( result.m_results.second );
}
#endif

namespace Be
{

template <typename T>
struct BlueWithStdResult
{
	typedef Be::Result<BeResultChoice<T, BlueStdResult>> type;
};

}
#endif