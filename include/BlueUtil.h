// Copyright © 2026 CCP ehf.

/* 
	*************************************************************************

	BlueUtil.h

	Author:    Matthias Gudmundsson
	Created:   Nov. 2000
	OS:        Win32
	Project:   Blue

	Description:   

		Utility functions, helper macros, templates,
		bedtime readings and more...


	Dependencies:

		Blue

	(c) CCP 2000

	*************************************************************************
*/

#ifndef _BLUEUTIL_H_
#define _BLUEUTIL_H_

#include "BlueTypes.h"

#if BLUE_WITH_PYTHON
#ifdef _DEBUG
bool GetBlueObjectRepr(IRoot* obj, PyObject* pyobj, char* reprbuff, size_t bufsize);
#endif
#endif


BLUEIMPORT Be::Var* BeMapMemberOffset(
	IRoot* obj, 
	const Be::VarEntry* var, 
	const Be::ClassInfo* typeinfo,
	ssize_t xtraoffs
	);


#define BLUEMAPMEMBEROFFSET(_obj, _var, _typeinfo, _offset)\
	BeMapMemberOffset(_obj, _var, _typeinfo, _offset)

#define BLUEMAPMEMBER(_obj, _var, _typeinfo) \
	BLUEMAPMEMBEROFFSET(_obj, _var, _typeinfo, 0)


class BLUEIMPORT BeTimer
{
public:
	
	BeTimer(const char *msg = NULL);
	void Reset();

	void LogTime(const char *msg = NULL) const;
		
	// Number of 100 nanosecs since last Reset()
	Be::Time GetTime() const;

	// Number of cpu cycles since last Reset()
	int64_t GetCycles() const;

	//frequency of the above cycles, cycles per second.
	static int64_t GetFreq();

	// Number of seconds since last Reset()
	double GetSeconds() const;
	
	Be::Time mStart;
	const char *mMsg;
};


//--------------------------------------------------------------------
// Blue allocator for stdlib.  Uses malloc/free, Yea!
// This defines our own allocator for use in the stdlib, which uses malloc
// and free.
// It also sublcasses some common containers for easy access, having this
// allocator as the default.
// Undefine the follwoing and you revert back to native std:: behaviour.
//--------------------------------------------------------------------
#define BLUE_STD_ALLOCATOR
#undef BLUE_STD_ALLOCATOR //.NET 2003 cannot compile the "rebind" thing.  bug!
#ifdef BLUE_STD_ALLOCATOR

#include <malloc.h>
#include <memory>

template<class T>
class BlueStdAllocator : public std::allocator<T> {
public:
	//constructor and copy constructors do nothing.  See definition of std::allocator.
	BlueStdAllocator() {}
	BlueStdAllocator(const BlueStdAllocator<T> &) {}
	template<class Other>
		BlueStdAllocator(const BlueStdAllocator<Other> &) {}
		
	T * allocate(size_t count, const void *hint = 0)
	{
		return (T*)malloc(count * sizeof(T));
	}
	
	void deallocate(T* ptr, size_t)
	{
		free(ptr);
	}

public:
	template<class _Other>
		struct rebind
		{	// convert an allocator<_Ty> to an allocator <_Other>
		typedef BlueStdAllocator<_Other> other;
		};
};

// Some subclassing of frequently used stl containers, to use BlueStdAllocator.
// We explicitly define constructors, so that they won't be implicitly created.

#include <vector>
template<
	class Key,
	class Alloc = BlueStdAllocator<Key> >
class BlueStdVector : public std::vector<Key, Alloc>
{
public:
	BlueStdVector() {}
	explicit BlueStdVector(size_t size) : std::vector<Key, Alloc>(size) {}
};

#include <list>
template<
	class Type,
	class Alloc = BlueStdAllocator<Type> >
class BlueStdList : public std::list<Type, Alloc>
{
public:
	BlueStdList() {}
};

#include <set>
template<
	class Key,
	class Traits = std::less<Key>,
	class Alloc = BlueStdAllocator<Key> >
class BlueStdSet : public std::set<Key, Traits, Alloc>
{
public:
	BlueStdSet() {}
};

template<
	class Key,
	class Traits = std::less<Key>,
	class Alloc = BlueStdAllocator<Key> >
class BlueStdMultiset : public std::multiset<Key, Traits, Alloc>
{
public:
	BlueStdMultiset() {}
};

#include <map>
template<
	class Key,
	class Type, 
	class Traits = std::less<Key>,
	class Alloc = BlueStdAllocator< std::pair<const Key, Type> > >
class BlueStdMap : public std::map<Key, Type, Traits, Alloc>
{
public:
	BlueStdMap() {}
};

#else // just use the regular stuff
#include <vector>
#include <list>
#include <map>
#include <set>

#define BlueStdAllocator std::allocator
#define BlueStdList std::list
#define BlueStdVector std::vector
#define BlueStdMap std::map
#define BlueStdSet std::set
#define BlueStdMultiset std::multiset

#endif //defined BLUE_STD_ALLOCATOR


// A function to compare two IRoot pointers for object identity.
// because of multiple inheritance of IRoot, this is not always
// easy.
inline bool BlueIsSame(IRoot *a, IRoot *b)
{
	if( !a || !b )
	{
		return false;
	}

	IRoot* va = a->GetRootObject();
	IRoot* vb = b->GetRootObject();

	return va == vb;
}

// Get the unique IRoot of the object
inline IRoot *BlueFinalIRoot(IRoot *a)
{
	return a->GetRootObject();
}

#if BLUE_WITH_PYTHON
BLUEIMPORT void BlueInitializePyType( PyTypeObject* pyType, const Be::Clsid* clsid, const char* doc, const Be::InterfaceEntry* interfaces, newfunc pyNew );
#endif

// Helper function for looking at command line arguments
std::vector<std::wstring> GetSplitCommandLine();

#endif
