// Copyright © 2026 CCP ehf.

#ifndef BlueStructureList_h
#define BlueStructureList_h

#include "BlueExposureMacros.h"
#include "IBlueStructureList.h"
#include "ICopier.h"

BLUE_DECLARE( StructureList );

#if BLUE_WITH_PYTHON

BLUEIMPORT PyObject* BlueStructureList_StructurePyObject( IBlueStructureList* structureList, uint8_t* item );
BLUEIMPORT void BlueStructureList_PyObjectToStructure( IBlueStructureList* structureList, PyObject* value, uint8_t* item );
BLUEIMPORT PyObject* BlueStructureList_PyGetStructureDefinition( IBlueStructureList* structureList );

#endif

BLUEIMPORT float BlueFloat16To32( uint16_t in );
BLUEIMPORT uint16_t BlueFloat32To16( float in );

namespace BlueStructureListHelpers
{
template <typename N>
struct IsBool { static const bool isBool = false; };

template <>
struct IsBool<bool> { static const bool isBool = true; };
}

template <typename T>
class BlueStructureList : 
	public IBlueStructureList,
	public ICopierCustomAssignment
{

	// Because STL vector has a smart specialization for std::vector<bool> we
	// cannot use it with BlueStructureList. In case one ever really needs it
	// he can write a specialization BlueStructureList<bool>.
	static_assert( !BlueStructureListHelpers::IsBool<T>::isBool, "BlueStructureList does not support bool as item type" );
public:
	typedef TrackableStdVector<T> vector_t;
	typedef typename vector_t::const_iterator const_iterator;
	typedef typename vector_t::iterator iterator;
	typedef typename vector_t::value_type value_type;
	typedef BlueStructureList<T> ClassDef;

	BlueStructureList( IRoot* lockobj = nullptr ) :
		m_structureDefinition( nullptr ),
		m_memberCount( 0 ),
		m_items("BlueStructureList"),
		m_defaultValue( nullptr ),
		m_notify( nullptr )
	{
	}

	~BlueStructureList()
	{}

	void SetStructureDefinition( BlueStructureDefinition* sd )
	{
		m_structureDefinition = sd;

		BlueStructureDefinition* memberDef = sd;
		m_memberCount = 0;
		while( memberDef->m_name )
		{
			++m_memberCount;
			++memberDef;
		}
	}

	void SetDefaultValue( const T* defaultValue )
	{
		m_defaultValue = defaultValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// IBlueStructureList methods
	///
	BlueStructureDefinition* GetStructureDefinition() override
	{
		return m_structureDefinition;
	}

	size_t GetMemberCount() override
	{
		return m_memberCount;
	}

	size_t GetStructureSize() override
	{
		return sizeof( T );
	}

	size_t GetSize() override
	{
		return m_items.size();
	}

	const void* GetDefaultValue() override
	{
		return m_defaultValue;
	}

	void* GetAt( size_t ix ) override
	{
		return &m_items[ix];
	}

	bool Append( const  void* val ) override
	{
		const T* p = static_cast<const T*>( val );
		m_items.push_back( *p );
		Notify( IBlueStructureListNotify::BLUE_STRUCTURE_LIST_INSERTED, val, m_items.size() - 1 );
		return true;
	}

	bool Insert( size_t ix, const void* val ) override
	{
		const T* p = static_cast<const T*>( val );
		iterator it = m_items.begin() + ix;
		m_items.insert( it, *p );
		Notify( IBlueStructureListNotify::BLUE_STRUCTURE_LIST_INSERTED, val, ix );
		return true;
	}

	bool Remove( size_t ix ) override
	{
		iterator it = m_items.begin() + ix;
		T item = m_items[ix];
		m_items.erase( it );
		Notify( IBlueStructureListNotify::BLUE_STRUCTURE_LIST_REMOVED, &item, ix );
		return true;
	}

	void Clear() override
	{
		m_items.clear();
		Notify( IBlueStructureListNotify::BLUE_STRUCTURE_LIST_CLEARED, nullptr, 0 );
	}

	void Resize( size_t numItems ) override
	{
		m_items.resize( numItems );
	}

	void ItemChanged( size_t ix ) override
	{
		Notify( IBlueStructureListNotify::BLUE_STRUCTURE_LIST_ITEM_CHANGED, &m_items[ix], ix );
	}

	IBlueStructureListNotify* SetNotify( IBlueStructureListNotify* notify ) override
	{
		auto old = m_notify;
		m_notify = notify;
		return old;
	}

	//////////////////////////////////////////////////////////////////////////
	// ICopierCustomAssignment

	// --------------------------------------------------------------------------------------
	// Description:
	//   Provides support for copying a BlueStructureList to another, through CloneTo() or CopyTo()
	// @@ ICopierCustomAssignment
	// --------------------------------------------------------------------------------------
	bool AssignTo(
		ICopierCustomAssignment* other,
		ICopier* copier
		) override
	{
		ClassDef* dest = static_cast<ClassDef*>( other );

		dest->m_items = m_items;

		return true;
	}

	//
	//////////////////////////////////////////////////////////////////////////

	const_iterator begin() const
	{
		return m_items.begin();
	}

	const_iterator end() const
	{
		return m_items.end();
	}

	iterator begin()
	{
		return m_items.begin();
	}

	iterator end()
	{
		return m_items.end();
	}

	size_t size() const
	{
		return m_items.size();
	}

	bool empty() const
	{
		return m_items.empty();
	}

	const T& operator[]( size_t ix ) const
	{
		return m_items[ix];
	}

	T& operator[]( size_t ix )
	{
		return m_items[ix];
	}

	void clear()
	{
		m_items.clear();
		Notify( IBlueStructureListNotify::BLUE_STRUCTURE_LIST_CLEARED, nullptr, 0 );
	}

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

	static PyObject* PyAppend( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		PyObject* obj;
		if( !PyArg_ParseTuple( args, "O", &obj ) )
		{
			return nullptr;
		}

		T item;

		try
		{
			BlueStructureList_PyObjectToStructure( pThis, obj, (uint8_t*)&item );
			pThis->Append( &item );
		}
		catch( std::exception& e )
		{
			PyErr_Format( PyExc_TypeError, "%s", e.what() );
			return nullptr;
		}
		Py_RETURN_NONE;
	}

	static PyObject* PyInsert( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		ssize_t index;
		PyObject* obj;
		if( !PyArg_ParseTuple( args, "nO", &index, &obj ) )
		{
			return nullptr;
		}
		auto length = pThis->GetSize();
		if( index > ssize_t( length ) )
		{
			index = ssize_t( length );
		}
		if( index < 0 )
		{
			index = length + index % length;
		}

		T item;

		try
		{
			BlueStructureList_PyObjectToStructure( pThis, obj, (uint8_t*)&item );
			pThis->Insert( index, &item );
		}
		catch( std::exception& e )
		{
			PyErr_Format( PyExc_TypeError, "%s", e.what() );
			return nullptr;
		}
		Py_RETURN_NONE;
	}

	static PyObject* PyRemoveAt( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		long index;
		if( !PyArg_ParseTuple( args, "l", &index ) )
		{
			return nullptr;
		}

		if( index < 0 || index >= long( pThis->size() ) )
		{
			PyErr_Format( PyExc_IndexError, "Structure list removeAt index %ld out of range", index );
			return nullptr;
		}
		else
		{
			pThis->Remove( size_t( index ) );
		}

		Py_RETURN_NONE;
	}

	static PyObject* PyGetStructureDefinition( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		if( !PyArg_ParseTuple( args, "" ) )
		{
			return nullptr;
		}

		return BlueStructureList_PyGetStructureDefinition( pThis );
	}

	static PyObject* PyGetDefaultValue( PyObject* self, PyObject* args )
	{
		ClassDef* pThis = BluePythonCast<ClassDef*>( self );

		if( !PyArg_ParseTuple( args, "" ) )
		{
			return nullptr;
		}

		auto defaultValue = pThis->GetDefaultValue();
		if( !defaultValue )
		{
			Py_RETURN_NONE;
		}
		return BlueStructureList_StructurePyObject( pThis, (uint8_t*)defaultValue );
	}
#endif

	EXPOSE_TO_BLUE()
	{
		EXPOSURE_BEGIN_IMP( ClassDef, "", GetStructureListClsid() )
			MAP_INTERFACE( IBlueStructureList )
			MAP_INTERFACE( ICopierCustomAssignment )

			MAP_METHOD
			( 
				"clear", 
				PyClear, 
				"Remove all items from the list"
			)

			MAP_METHOD
			(
				"append",
				PyAppend,
				"Appends a value to the list"
			)

			MAP_METHOD
			(
				"insert",
				PyInsert,
				"Inserts a value to the list"
			)

			MAP_METHOD
			(
				"removeAt",
				PyRemoveAt,
				"Removes element at the given index\nArguments:\nindex - element index to remove"
			)

			MAP_METHOD
			(
				"GetStructureDefinition",
				PyGetStructureDefinition,
				"Gets the structure definition"
			)

			MAP_METHOD
			(
				"GetDefaultValue",
				PyGetDefaultValue,
				"Returns a tuple that is considered a default item value for the list or None if the\n"
				"list does not have a specific default value"
			)
		EXPOSURE_END()
	}


private:
	void Notify( IBlueStructureListNotify::Event event, const void* item, size_t index )
	{
		if( m_notify )
		{
			m_notify->OnStructureListModified( event, item, index, this );
		}
	}
	BlueStructureDefinition* m_structureDefinition;
	size_t m_memberCount;
	vector_t m_items;
	const T* m_defaultValue;
	IBlueStructureListNotify* m_notify;
};

#endif // BlueStructureList_h
