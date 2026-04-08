// Copyright © 2026 CCP ehf.

/* 
	*************************************************************************

	BluePythonThunkers.h

	Author:    Matthias Gudmundsson
	Created:   Nov. 2000
	OS:        Win32
	Project:   Blue

	Description:   

		Generic interface thunkers


	Dependencies:

		Blue

	(c) CCP 2000

	*************************************************************************
*/

#ifndef _BLUEPYTHONTHUNKERS_H_
#define _BLUEPYTHONTHUNKERS_H_

#if BLUE_WITH_PYTHON

#include "include/IList.h"
#include "include/BlueExposureMacros.h"
#include "include/BlueExposureMacrosDeprecated.h"

//////////////////////////////////////////////////////////////////////
//
// IRoot Thunkers
//
//////////////////////////////////////////////////////////////////////
class IRoot_Thunk : public IRoot
{
public:

	typedef IRoot_Thunk _Class;
	typedef IRoot _Interface;

	static const Be::IID& IID()
	{
		static Be::IID iid( "IRoot" );
		return iid;
	}

	const Be::Clsid* Clsid()
	{
		return ClassType()->mClassId;
	}

	static const PyMethodDef* Defs()
	{
		THUNKER_BEGIN()
			MAPPYTHON( CopyTo,		"Copies data from this object")
			MAPPYTHON( CloneTo,		"Clones this object, preserving topology")
			MAPPYTHON( TypeInfo,	"Returns various type info for this object")
			MAPPYTHON( Find,		
				"Find a Trinity class type, or list of path objects to it"
				"\n"
				"\nKeyword arguments:"
				"\nclass name -- class name or list of class names, e.g. ['trinity.TriEffect']"
				"\nmaxLevel -- maximum depth to search to (default -1)"
				"\nprune -- prune duplicate instances (default False)"
				"\nnParents -- number of parents to return in a list (default -1)"
			)
			MAPPYTHON( Validate,	"Traverses hierarchy and validates")
			MAPPYTHON( GetRefCounts,"Returns the python and blue reference counts")
		THUNKER_END()
	}

	// compatible python methods
	DECLARE_PYMETHODTHUNK( CopyTo );
	DECLARE_PYMETHODTHUNK( CloneTo );
	DECLARE_PYMETHODTHUNK( TypeInfo );
	DECLARE_PYMETHODTHUNK( Find );
	DECLARE_PYMETHODTHUNK( Validate );
	DECLARE_PYMETHODTHUNK( GetRefCounts );


private:
	PyObject* CloneCopyImpl(PyObject *args, bool clone);

};



//////////////////////////////////////////////////////////////////////
//
// IList Thunkers
//
//////////////////////////////////////////////////////////////////////
class IList_Thunk : public IList
{
public:

	typedef IList_Thunk _Class;
	typedef IList _Interface;

	static const Be::IID& IID()
	{
		static Be::IID iid( "IList" );
		return iid;
	}

	const Be::Clsid* Clsid()
	{
		return ClassType()->mClassId;
	}

	static const PyMethodDef* Defs()
	{
		THUNKER_BEGIN()
			// compatible python methods
			MAPPYTHON( append,		"L.append(object) -- append object to end")
			MAPPYTHON( insert,		"L.insert(index, object) -- insert object before index")
			MAPPYTHON( extend,		"L.extend(list) -- extend list by appending list elements")
			MAPPYTHON( pop,			"L.pop([index]) -> item -- remove and return item at index (default last)")
			MAPPYTHON( remove,		"L.remove(value) -- remove first occurrence of value")
			MAPPYTHON( removeAt,	"L.removeAt(index) -- remove value at index, if -1 all values are removed")
			MAPPYTHON( index,		"L.index(value) -> integer -- return index of first occurrence of value")
			MAPPYTHON( count,		"L.count(value) -> integer -- return number of occurrences of value")
			MAPPYTHON( reverse,		"L.reverse() -- reverse *IN PLACE*")
			MAPPYTHON( sort,		"L.sort([cmpfunc]) -- sort *IN PLACE*; if given, cmpfunc(x, y) -> -1, 0, 1")
			MAPPYTHON( fremove,		"L.fremove(value) -- returns true if object was removed.  list order is not preserved.")

			MAPPYTHON( FindByName,	"L.FindByName(name) -- returns list element with name attribute equal to passed name or None if not found.")

			MAPPYTHON( GetInfo,		"GetInfo")

		THUNKER_END()
	}

	// compatible python methods
	DECLARE_PYMETHODTHUNK( append );
	DECLARE_PYMETHODTHUNK( insert );
	DECLARE_PYMETHODTHUNK( extend );
	DECLARE_PYMETHODTHUNK( pop );
	DECLARE_PYMETHODTHUNK( remove );
	DECLARE_PYMETHODTHUNK( removeAt );
	DECLARE_PYMETHODTHUNK( index );
	DECLARE_PYMETHODTHUNK( count );
	DECLARE_PYMETHODTHUNK( reverse );
	DECLARE_PYMETHODTHUNK( sort );
	DECLARE_PYMETHODTHUNK( fremove );

	DECLARE_PYMETHODTHUNK( FindByName );

	DECLARE_PYMETHODTHUNK( GetInfo );
};

#endif

#endif
