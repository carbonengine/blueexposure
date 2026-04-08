// Copyright © 2026 CCP ehf.

#pragma once
#ifndef INotify_h
#define INotify_h

#include "BlueTypes.h"

BLUE_DECLARE_INTERFACE(INotify);

BLUE_INTERFACE( INotify ) : public IRoot
{
	// Called when a member of an instance is modified.
	// Usually it's Python or Jennifer which calls this
	// function.
	// This function is only called for members that are
	// flagged with Be::NOTIFY in the members mapping.
	//
	// The preferred way of comparing 'value' to local class
	// members is like this:
	//
	// if ((Be::Var*)&mMyMember == value) do smth...
	//
	// NOTE! When comparing smart pointers and BlueStr, make
	// sure to do the comparison on the member of the wrapper.
	// (The same rule as in the member mapping macros).
	//
	// if ((Be::Var*)&mMySmartPointer.p == value) do smth...
	//
	// if ((Be::Var*)&mMyBlueString.mStr == value) do smth...
	//
	virtual bool OnModified(
		Be::Var* value
		) = 0;
};

template<typename T>
inline bool IsMatch( Be::Var* value, const T& t, typename std::enable_if<std::is_fundamental<T>::value, T>::type* = 0 )
{
	return (Be::Var*)&t == value;
}

template<typename T>
inline bool IsMatch( Be::Var* value, const T& t, typename std::enable_if<std::is_enum<T>::value, T>::type* = 0 )
{
	return (Be::Var*)&t == value;
}

template<typename T>
inline bool IsMatch( Be::Var* value, const BluePtr<T>& t )
{
	return ( Be::Var* )&t.p == value;
}

inline bool IsMatch( Be::Var* value, const IRootPtr& t )
{
	return ( Be::Var* )&t.p == value;
}

inline bool IsMatch( Be::Var* value, const std::string& t )
{
	return (Be::Var*)&t == value;
}

inline bool IsMatch( Be::Var* value, const std::wstring& t )
{
	return (Be::Var*)&t == value;
}

inline bool IsMatch( Be::Var* value, const BlueSharedString& t )
{
	return (Be::Var*)&t == value;
}

inline bool IsMatch( Be::Var* value, const BlueSharedStringW& t )
{
	return (Be::Var*)&t == value;
}


#endif // INotify_h
