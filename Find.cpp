// Copyright © 2026 CCP ehf.

#if BLUE_WITH_PYTHON

#include "Find.h"

#include <vector>
#include "include/BluePythonObject.h"
#include "include/BlueUtil.h"

namespace
{


class SeenObjectSet
{
public:
	SeenObjectSet()
	{
		m_buckets.resize( 1024 );
	}

	bool Visit( IRoot* obj )
	{
		auto h = std::hash<IRoot*>()( obj );
		auto& bucket = m_buckets[h % m_buckets.size()];
		auto found = std::find( begin( bucket ), end( bucket ), obj );
		if( found == end( bucket ) )
		{
			bucket.push_back( obj );
			return true;
		}
		return false;
	}

private:
	std::vector<std::vector<IRoot*>> m_buckets;
};


struct StackEntry
{
	StackEntry( IRoot* o, int l, bool c ) :
		obj( o->GetRootObject() ), level( l ), check( c )
	{
	}

	IRoot* obj; //pointer to the IRoot
	int level; //Recursion level of this object
	bool check; //Should we check if we have seen it before (i.e. is this
		//a proper IRoot pointer and not an auto variable?)
};


typedef std::vector<StackEntry> objectstack_t;
typedef std::vector<Be::Clsid> idvector_t;


struct RouteStackEntry
{
	explicit RouteStackEntry( IRoot* root ) :
		obj( root->GetRootObject() ),
		level( 0 ),
		parent( -1 ),
		attr( nullptr ),
		index( 0 )
	{
	}
	RouteStackEntry( IRoot* child, ssize_t level_, ssize_t parent_, const Be::ClassInfo*, const Be::VarEntry* entry, ptrdiff_t ) :
		obj( child->GetRootObject() ),
		level( level_ ),
		parent( parent_ ),
		attr( entry ),
		index( 0 )
	{
	}
	RouteStackEntry( IRoot* child, ssize_t level_, ssize_t parent_, ssize_t index_ ) :
		obj( child->GetRootObject() ),
		level( level_ ),
		parent( parent_ ),
		attr( nullptr ),
		index( index_ )
	{
	}

	IRoot* obj;
	ssize_t level;
	ssize_t parent;
	const Be::VarEntry* attr;
	ssize_t index;
};


struct RouteStepStackEntry : public RouteStackEntry
{
	explicit RouteStepStackEntry( IRoot* root ) :
		RouteStackEntry( root ),
		type( nullptr ),
		offset( 0 )
	{
	}
	RouteStepStackEntry( IRoot* child, ssize_t level_, ssize_t parent_, const Be::ClassInfo* type_, const Be::VarEntry* entry, ptrdiff_t offset_ ) :
		RouteStackEntry( child, level_, parent_, type_, entry, offset_ ),
		type( type_ ),
		offset( offset_ )
	{
	}
	RouteStepStackEntry( IRoot* child, ssize_t level_, ssize_t parent_, ssize_t index_ ) :
		RouteStackEntry( child, level_, parent_, index_ ),
		type( nullptr ),
		offset( 0 )
	{
	}

	RouteStep GetRouteStep( const std::vector<RouteStepStackEntry> stack ) const
	{
		auto p = stack[parent].obj;
		RouteStep step;
		if( attr )
		{
			RouteStep::StepValue value = { std::string(), 0, { type, attr, offset } };
			step = RouteStep( RouteStep::ATTRIBUTE, value, p );
		}
		else if( IBlueDictPtr dict = BlueCastPtr( p ) )
		{
			RouteStep::StepValue value = { dict->GetKey( index ), 0, {} };
			step = RouteStep( RouteStep::KEY, value, p );
		}
		else
		{
			RouteStep::StepValue value = { std::string(), index, {} };
			step = RouteStep( RouteStep::INDEX, value, p );
		}
		return step;
	}

	const Be::ClassInfo* type;
	ptrdiff_t offset;
};


bool IsMatch( IRoot* obj, const idvector_t& clsids )
{
	// do we have a winner here?  See if the class matches what we are looking for.
	const Be::ClassInfo* type = obj->ClassType();
	for( const Be::ClassInfo* other = type; other; other = other->mParentClassInfo )
		for( idvector_t::const_iterator it = clsids.begin(); it != clsids.end(); ++it )
			if( ( *it ).IsEqual( *other->mClassId ) )
				return true;
	return false;
}


bool IsMatch( IRoot* obj, const Be::IID& iid )
{
	auto type = obj->ClassType();
	for( auto other = type; other; other = other->mParentClassInfo )
	{
		for( auto entry = type->mInterfaceTable; entry->mIID; entry++ )
		{
			if( entry->mIID->IsEqual( iid ) )
			{
				return true;
			}
		}
	}
	return false;
}


PyObject* GetPathItem( IRoot* parent, const Be::VarEntry* attr, ssize_t index )
{
	if( attr )
	{
		return Py_BuildValue( "(Nis)", BlueWrapObjectForPython( parent ), 0, attr->mName );
	}
	else if( IBlueDictPtr dict = BlueCastPtr( parent ) )
	{
		return Py_BuildValue( "(Nis)", BlueWrapObjectForPython( parent ), 1, dict->GetKey( index ) );
	}
	else
	{
		return Py_BuildValue( "(Nii)", BlueWrapObjectForPython( parent ), 1, index );
	}
}


PyObject* FindSingle( IRoot* obj, const idvector_t& ids, int maxLevel, bool prune )
{
	objectstack_t stack;
	SeenObjectSet seen;

	PyObject* result = PyList_New( 0 );
	if( !result )
		return 0;

	//Prime the stack and start
	stack.push_back( StackEntry( obj, 0, true ) ); // 'true' since we must assume it's not an autovar.
	while( !stack.empty() )
	{
		StackEntry e = stack.back();
		stack.pop_back();

		obj = e.obj;
		if( e.check )
		{
			if( !seen.Visit( obj ) )
			{
				continue;
			}
		}

		// do we have a winner here?  See if the class matches what we are looking for.
		if( IsMatch( obj, ids ) )
		{
			PyObject* item = BlueWrapObjectForPython( obj );
			if( !item || PyList_Append( result, item ) )
			{
				Py_XDECREF( item );
				Py_DECREF( result );
				return 0;
			}
			Py_DECREF( item );

			if( prune )
				continue; //don't look below a match
		}

		//now, depending on the level, we go on and search the children
		if( maxLevel >= 0 && e.level >= maxLevel )
			continue; //no, we don't go any deeper.

		EnumerateChildren(
			obj,
			[&]( IRoot* child, auto, auto entry, auto ) { stack.push_back( StackEntry( child, e.level + 1, entry->mType == Be::IROOTPTR ) ); },
			[&]( IRoot* child, auto index ) { stack.push_back( StackEntry( child, e.level + 1, true ) ); },
			[&]( IRoot* child, auto index ) { stack.push_back( StackEntry( child, e.level + 1, true ) ); },
			[]( const Be::VarEntry* entry ) { return ( entry->mEditFlags & Be::PERSIST ) != 0; } );
	}
	return result;
}


template <typename StackEntryType, typename Op>
void EnumerateRoutes( IRoot* from, IRoot* to, const Op& operation )
{
	std::vector<StackEntryType> stack;

	from = from->GetRootObject();
	to = to->GetRootObject();
	if( from == to )
	{
		operation( stack );
		return;
	}

	SeenObjectSet seen;

	stack.push_back( StackEntryType( from ) );
	ssize_t prevLevel = 0;

	while( !stack.empty() )
	{
		auto& e = stack.back();
		if( e.level < prevLevel )
		{
			prevLevel = e.level;
			stack.pop_back();
			continue;
		}
		prevLevel = e.level + 1;

		auto obj = e.obj;
		if( obj == to )
		{
			if( !operation( stack ) )
			{
				return;
			}
		}
		else
		{
			if( !seen.Visit( obj ) )
			{
				continue;
			}
			ssize_t parentIdx = ssize_t( stack.size() ) - 1;
			auto level = e.level + 1;
			EnumerateChildren(
				obj,
				[&]( IRoot* child, auto type, auto entry, auto offset ) {
					stack.push_back( StackEntryType( child, level, parentIdx, type, entry, offset ) );
				},
				[&]( IRoot* child, auto index ) {
					stack.push_back( StackEntryType( child, level, parentIdx, index ) );
				},
				[&]( IRoot* child, auto index ) {
					stack.push_back( StackEntryType( child, level, parentIdx, index ) );
				},
				[]( auto ) { return true; } );
		}
	}
}

}


PyObject* PyFindImpl( IRoot* pThis, PyObject* args )
{
	PyObject *clsidobj;
	int maxLevel = -1;
	int bPrune = 0;
	if (!PyArg_ParseTuple(args, "O|ii", &clsidobj, &maxLevel, &bPrune))
		return NULL;

	// parse first argument.  Either a string, or a sequence of strings.
	idvector_t clsids;
	if (PyString_Check(clsidobj)) {
		Be::Clsid clsid;
		if (!clsid.InitFromString(PyString_AsString(clsidobj)))
			return 0;
		clsids.push_back(clsid);
	} else {
		BluePySeq seq(BluePy(clsidobj, true));
		if (!seq.Check()) {
			PyErr_SetString(PyExc_TypeError, "first argument must be string or sequence of strings");
			return 0;
		}
		Py_ssize_t s = seq.Size();
		for(Py_ssize_t i = 0; i< s; i++) {
			BluePyStr str = seq.Get(i);
			if (!str.Check()) {
				PyErr_SetString(PyExc_TypeError, "first argument must be string or sequence of strings");
				return 0;
			}
			Be::Clsid clsid;
			if (!clsid.InitFromString(str.Str()))
				return 0;
			clsids.push_back(clsid);
		}
	}

	return FindSingle( pThis, clsids, maxLevel, !!bPrune );
}


std::vector<IRootPtr> FindInterface( IRoot *obj, const char* iidName )
{
	Be::IID iid( iidName );

	objectstack_t stack;
	SeenObjectSet seen;

	std::vector<IRootPtr> result;

	if( !obj )
	{
		return result;
	}

	//Prime the stack and start
	stack.push_back( StackEntry( obj, 0, true ) );  // 'true' since we must assume it's not an autovar.
	while( !stack.empty() )
	{
		StackEntry e = stack.back();
		stack.pop_back();

		obj = e.obj;
		if( e.check )
		{
			// We need to check this, if we've seen it before
			if( !seen.Visit( obj ) )
			{
				continue;
			}
		}

		// do we have a winner here?  See if the class matches what we are looking for.
		if( IsMatch( obj, iid ) )
		{
			result.push_back( obj );
		}

		EnumerateChildren(
			obj, [&]( IRoot* child, const Be::VarEntry* entry, ssize_t index ) {
				stack.push_back( StackEntry( child, e.level + 1, !entry || entry->mType == Be::IROOTPTR ) );
			} );
	}

	return result;
}



bool FindFirstRoute( IRoot* from, IRoot* to, std::vector<RouteStep>* result )
{
	if( !from || !to )
	{
		return false;
	}

	bool route_found = false;

	std::vector<RouteStepStackEntry> stack;

	from = from->GetRootObject();
	to = to->GetRootObject();
	if( from == to )
	{
		return true;
	}

	EnumerateRoutes<RouteStepStackEntry>( from, to, [&]( const auto& stack ) {
		route_found = true;
		if( result )
		{
			ssize_t idx = stack.size() - 1;
			while( idx > 0 )
			{
				auto& i = stack[idx];
				result->insert( result->begin(), stack[idx].GetRouteStep( stack ) );
				idx = i.parent;
			}
		}
		return false;
	} );
	return route_found;
}


BluePy FindRoute( IRoot* from, IRoot* to )
{
	if( !from || !to )
	{
		return PyErr_SetString( PyExc_ValueError, "neither from nor to parameter can be None" ), BluePy();
	}

	PyObject* result = PyList_New( 0 );

	EnumerateRoutes<RouteStackEntry>( from, to, [&]( const auto& stack ) {
		PyObject* path = PyList_New( 0 );
		ssize_t idx = stack.size() - 1;
		while( idx > 0 )
		{
			auto& i = stack[idx];
			auto& p = stack[i.parent];
			PyObject* itm = GetPathItem( p.obj, i.attr, i.index );
			PyList_Insert( path, 0, itm );
			Py_DECREF( itm );
			idx = i.parent;
		}
		PyList_Append( result, path );
		Py_DECREF( path );
		return true;
	} );

	return BluePy( result );
}


BluePy PyFindFirstRoute( IRoot* from, IRoot* to )
{
	if( !from || !to )
	{
		PyErr_Format( PyExc_TypeError, "Argument 'from' or 'to' are not of type IRoot." );
		return BluePy();
	}

	from = from->GetRootObject();
	to = to->GetRootObject();
	if( from == to )
	{
		return BluePy( PyList_New( 0 ) );
	}

	PyObject* path = nullptr;
	EnumerateRoutes<RouteStackEntry>( from, to, [&]( const auto& stack ) {
		path = PyList_New( 0 );
		ssize_t idx = stack.size() - 1;
		while( idx > 0 )
		{
			auto& i = stack[idx];
			auto& p = stack[i.parent];
			PyObject* itm = GetPathItem( p.obj, i.attr, i.index );
			PyList_Insert( path, 0, itm );
			Py_DECREF( itm );
			idx = i.parent;
		}
		return false;
	} );

	if( path )
	{
		return BluePy( path );
	}

	PyErr_Format( PyExc_ValueError, "No valid route found." );
	return BluePy();
}


bool FindReference( IRoot* from, IRoot* to )
{
	if( from == to )
	{
		return true;
	}
	if( !from || !to )
	{
		return false;
	}

	objectstack_t stack;
	SeenObjectSet seen;

	//Prime the stack and start
	stack.push_back( StackEntry( from, 0, true ) ); // 'true' since we must assume it's not an autovar.
	while( !stack.empty() )
	{
		StackEntry e = stack.back();
		stack.pop_back();

		from = e.obj;
		if( e.check )
		{
			// We need to check this, if we've seen it before
			if( !seen.Visit( from ) )
				continue;
		}

		// do we have a winner here?  See if the class matches what we are looking for.
		if( from == to )
		{
			return true;
		}

		EnumerateChildren(
			from, [&]( IRoot* child, const Be::VarEntry* entry, ssize_t index ) {
				stack.push_back( StackEntry( child, e.level + 1, !entry || entry->mType == Be::IROOTPTR ) );
			} );
	}
	return false;
}


BluePy FindAllReferences( IRoot* root )
{
	PyObject* result = PyDict_New();

	root = root->GetRootObject();
	if( !root || !result )
	{
		return BluePy( result );
	}

	std::vector<StackEntry> stack;
	SeenObjectSet seen;

	//Prime the stack and start
	stack.push_back( StackEntry( root, 0, true ) ); // 'true' since we must assume it's not an autovar.
	while( !stack.empty() )
	{
		auto obj = stack.back();
		stack.pop_back();

		if( !seen.Visit( obj.obj ) )
		{
			continue;
		}

		auto start = stack.size();

		EnumerateChildren(
			obj.obj,
			[&]( IRoot* child, const Be::VarEntry* entry, ssize_t index ) {
				stack.push_back( StackEntry( child, obj.level + 1, !entry || entry->mType == Be::IROOTPTR ) );

				auto record = GetPathItem( obj.obj, entry, index );

				auto pyChild = BlueWrapObjectForPython( child );
				auto found = PyDict_GetItem( result, pyChild );
				if( !found )
				{
					found = PyList_New( 0 );
					PyDict_SetItem( result, pyChild, found );
					Py_DECREF( found );
				}
				PyList_Append( found, record );
				Py_DECREF( record );
				Py_DECREF( pyChild );
			} );
	}
	return BluePy( result );
}


RouteStep::RouteStep() :
	m_stepType( StepType::INDEX ),
	m_value(),
	m_obj( nullptr )
{
}

RouteStep::RouteStep( StepType stepType, const StepValue& value, IRoot* root ) :
	m_stepType( stepType ),
	m_obj( root )
{
	switch( stepType )
	{
	case RouteStep::StepType::ATTRIBUTE:
		m_value.attribute = value.attribute;
		break;
	case RouteStep::StepType::INDEX:
		m_value.index = value.index;
		break;
	case RouteStep::StepType::KEY:
		m_value.key = value.key;
		break;
	default:
		break;
	}
}


RouteStep::RouteStep( const RouteStep& ref ) :
	RouteStep( ref.m_stepType, ref.m_value, ref.m_obj )
{
}

RouteStep::~RouteStep()
{
}

IRoot* RouteStep::GetNextObject( IRoot* parent )
{
	if( !parent )
	{
		parent = m_obj;
	}
	switch( m_stepType )
	{
	case StepType::ATTRIBUTE:
		{
			const Be::ClassInfo* type = parent->ClassType();
			for( const Be::ClassInfo* current = type; current; current = current->mParentClassInfo )
			{
				if( current == m_value.attribute.type )
				{
					Be::Var* var = BLUEMAPMEMBEROFFSET( parent, m_value.attribute.entry, m_value.attribute.type, m_value.attribute.offset );
					if( m_value.attribute.entry->mType == Be::IROOTPTR )
					{
						return var->mIRootPtr;
					}
					else
					{
						return reinterpret_cast<IRoot*>( var );
					}
					break;
				}
			}
		}
		break;
	case StepType::INDEX:
		{
			IListPtr list( BlueCastPtr( parent ) );
			if( list && list->GetSize() > m_value.index )
			{
				return list->GetAt( m_value.index );
			}
		}
		break;
	case StepType::KEY:
		{
			IBlueDictPtr dict( BlueCastPtr( parent ) );
			if( dict && dict.p )
			{
				return dict.p->Subscript( m_value.key.c_str() );
			}
		}
		break;
	default:
		break;
	}
	return nullptr;
}

#endif
