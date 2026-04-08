// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueDict_h
#define BlueDict_h

#include "IBlueDict.h"
#include "ICopier.h"
#include "BlueExposureMacros.h"

BLUE_DECLARE( Dict );

template <class T>
class BlueDict: 
	public IBlueDict, 
	public ICopierCustomAssignment
{
public:

	BlueDict( IRoot* lockobj = NULL ) : 
		m_entries( "BlueDict" ),
		m_iterators( "BlueDict" ),
		m_iteratorsValid( false )
	{
	}

	~BlueDict()
	{
		clear();
	}

	typedef TrackableStdHashMap<std::string, T*> Map_t;
	typedef typename Map_t::iterator iterator;
	typedef typename Map_t::const_iterator const_iterator;
	typedef typename Map_t::value_type value_type;

	const_iterator begin() const
	{
		return m_entries.begin();
	}

	const_iterator end() const
	{
		return m_entries.end();
	}

	size_t size() const
	{
		return m_entries.size();
	}

	bool empty() const
	{
		return m_entries.empty();
	}

	T* operator[]( const char* key ) const
	{
		return static_cast<T*>( Subscript( key ) );
	}

	void clear()
	{
		for( const_iterator it = m_entries.begin(); it != m_entries.end(); ++it )
		{
			it->second->Unlock();
		}
		m_entries.clear();
		m_iterators.clear();
		m_iteratorsValid = false;
	}

	const_iterator find( const char* key ) const
	{
		return m_entries.find( key );
	}

	std::pair<const_iterator, bool> insert( const value_type& val )
	{
		return m_entries.insert( val );
	}


	//////////////////////////////////////////////////////////////////////////
	// IBlueDict

	size_t GetLength() const override
	{
		return m_entries.size();
	}

	IRoot* Subscript( const char* key ) const override
	{
		// Note that don't want to use the operator[] as it will
		// insert a default value into the map if one isn't found.
		const_iterator it = m_entries.find( key );
		if( it != m_entries.end() )
		{
			return it->second;
		}
		return NULL;
	}

	bool AssignSubscript( const char* key, IRoot* value ) override
	{
		if( m_iteratorsValid )
		{
			m_iterators.clear();
			m_iteratorsValid = false;
		}

		iterator it = m_entries.find( key );
		if( it != m_entries.end() )
		{
			it->second->Unlock();
		}

		if( value )
		{
			T* tmp = NULL;
			static Be::IID irootType( "IRoot" );
			value->QueryInterface( irootType, (void**)&tmp );
			m_entries[key] = tmp;
		}
		else
		{
			// Assigning None is the same as deleting the entry
			m_entries.erase( it );
		}
		return true;
	}

	const char* GetKey( size_t index ) override
	{
		if( !m_iteratorsValid )
		{
			CCP_ASSERT( m_iterators.empty() );

			m_iterators.reserve( m_entries.size() );
			for( const_iterator it = m_entries.begin(); it != m_entries.end(); ++it )
			{
				m_iterators.push_back( it );
			}
			m_iteratorsValid = true;
		}

		if( index >= m_iterators.size() )
		{
			return NULL;
		}

		return m_iterators[index]->first.c_str();
	}

	typedef BlueDict<T> ClassDef;

#if BLUE_WITH_PYTHON
	//////////////////////////////////////////////////////////////////////////
	// Python exposure
	//

	// Note that MAP_METHOD_AND_WRAP chokes on a templated class so
	// we have to create these thunkers manually.

	static PyObject* PyClear( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		pThis->clear();

		Py_RETURN_NONE;
	}

	static PyObject* PyIterKeys( PyObject* self, PyObject* args )
	{
		return PySeqIter_New( self );
	}

	static PyObject* PyGet( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		const char* key = NULL;
		PyObject* defaultValue = NULL;
		if( !PyArg_ParseTuple( args, "s|O", &key, &defaultValue ) )
		{
			return NULL;
		}

		IRoot* p = pThis->Subscript( key );
		if( p )
		{
			Py_XDECREF( defaultValue );
			return BlueWrapObjectForPython( p );
		}

		if( defaultValue )
		{
			Py_XINCREF( defaultValue );
			return defaultValue;
		}

		Py_RETURN_NONE;
	}

	static PyObject* PyItems( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		size_t len = pThis->GetLength();
		PyObject* returnValue = PyList_New( len );

		for( size_t i = 0; i < len; ++i )
		{
			const char* key = pThis->GetKey( i );
			CCP_ASSERT( key );

			IRoot* p = pThis->Subscript( key );
			CCP_ASSERT( p );

			PyObject* obj = BlueWrapObjectForPython( p );

			PyObject* tuple = PyTuple_New( 2 );
			PyTuple_SET_ITEM( tuple, 0, PyString_FromString( key ) );
			PyTuple_SET_ITEM( tuple, 1, obj );

			PyList_SET_ITEM( returnValue, i, tuple );
		}

		return returnValue;
	}

	static PyObject* PyKeys( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		size_t len = pThis->GetLength();
		PyObject* returnValue = PyList_New( len );

		for( size_t i = 0; i < len; ++i )
		{
			const char* key = pThis->GetKey( i );
			CCP_ASSERT( key );

			PyList_SET_ITEM( returnValue, i, PyString_FromString( key ) );
		}

		return returnValue;
	}

	static PyObject* PyValues( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		size_t len = pThis->GetLength();
		PyObject* returnValue = PyList_New( len );

		for( size_t i = 0; i < len; ++i )
		{
			const char* key = pThis->GetKey( i );
			CCP_ASSERT( key );

			IRoot* p = pThis->Subscript( key );
			CCP_ASSERT( p );

			PyObject* obj = BlueWrapObjectForPython( p );

			PyList_SET_ITEM( returnValue, i, obj );
		}

		return returnValue;
	}
#endif


	EXPOSE_TO_BLUE()
	{
		EXPOSURE_BEGIN_IMP( ClassDef, "", GetDictClsid() )
			MAP_INTERFACE( IBlueDict )
			MAP_INTERFACE( ICopierCustomAssignment )

			MAP_METHOD
			( 
				"clear", 
				BlueDict<T>::PyClear, 
				"Remove all items from the dictionary"
			)
			MAP_METHOD
			( 
				"iterkeys", 
				BlueDict<T>::PyIterKeys,
				"Return an iterator over the dictionary's keys."
			)
			MAP_METHOD
			( 
				"get", 
				BlueDict<T>::PyGet,
				"get( key[, default])\n"
				"  Return the value for 'key' if 'key' is in the dictionary, else 'default'\n"
				"  If 'default' is not given, it defaults to None, so that this method\n"
				"  never raises a KeyError."
			)
			MAP_METHOD
			(
				"items",
				BlueDict<T>::PyItems,
				"Return a copy of the dictionary's list of (key, value) pairs."
			)
			MAP_METHOD
			(
				"keys",
				BlueDict<T>::PyKeys,
				"Return a copy of the dictionary's list of keys."
			)
			MAP_METHOD
			(
				"values",
				BlueDict<T>::PyValues,
				"Return a copy of the dictionary's list of values."
			)
		EXPOSURE_END()
	}

// --------------------------------------------------------------------------------------
// Description:
//   Provides support for copying a BlueDict to another, through CloneTo() or CopyTo()
// @@ ICopierCustomAssignment
// --------------------------------------------------------------------------------------
	bool AssignTo(
		ICopierCustomAssignment* other,
		ICopier* copier
		) override
	{
		ClassDef* dest = static_cast<ClassDef*>( other );

		dest->clear();

		for ( iterator i = m_entries.begin(); i != m_entries.end(); ++i )
		{

			IRoot* tmp = NULL;
			if( !copier->CopyTo( i->second, &tmp) )
			{
				continue;
			}
			// There's no reference counting internal to a BlueDict
			// so allow it to 'steal' the first reference we just got back
			dest->insert( std::make_pair( i->first, static_cast<T*>( tmp ) ) );
		}

		return true;
	}

private:
	typedef TrackableStdVector<typename Map_t::const_iterator> IteratorVector_t;

	Map_t m_entries;

	// Python iterates over a dict via indexing - to support this we keep a
	// vector of iterators into the map.
	IteratorVector_t m_iterators;

	bool m_iteratorsValid;
};


#endif // BlueDict_h