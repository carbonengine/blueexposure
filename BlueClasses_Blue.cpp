// Copyright © 2026 CCP ehf.

#include "include/BlueTypes.h"
#include "include/BlueRegistration.h"
#include "include/IInitialize.h"
#include "BlueClasses.h"
#if BLUE_WITH_PYTHON
#include "include/BlueSmartPy.h"
#include "include/BluePythonObject.h"
#include "BluePyWrap.h"
#include "Find.h"
#include "TypeInfo.h"
#endif

static bool GetBeClasses( const Be::IID& riid, void** ppv )
{
	return BeClasses->QueryInterface( riid, ppv );
}

BLUE_DEFINE_NO_REGISTER( BlueClasses );
BLUE_REGISTER_CLASS_EX( BlueClasses, GetBeClasses, Be::ClassRegistration::DISABLE_PYTHON_CONSTRUCTION );

#if BLUE_WITH_PYTHON
PyObject* BlueClasses::PyCreateInstance( PyObject* self, PyObject* args )
{
	const char* classid;
	PyObject* xArgs = NULL;

	if( !PyArg_ParseTuple(args, "s|O:PyCreateInstance", &classid,&xArgs) )
	{
		return NULL;
	}

	if (xArgs != NULL)
	{
		if (!PySequence_Check(xArgs))
		{
			PyErr_SetString( PyExc_TypeError, "Extra arguments must be sequence" );
		}
	}

	IRootPtr obj;

	if( !obj.CreateInstance( classid ) )
	{
		return NULL;
	}

	IInitializePtr init( BlueCastPtr( obj ) );
	if ( init )
	{
		if( !init->Initialize() )
		{
			return NULL;
		}
	}

	BluePy ret( BlueWrapObjectForPython( obj ) );
	BluePy initCall( PyObject_GetAttrString( ret, "__init__" ) );
	if( initCall )
	{
		BluePy initResult( PyObject_CallObject( initCall,xArgs ) );
		if( !initResult )
		{
			return NULL;
		}
	}
	else
	{
		PyErr_Clear();
	}
	return ret.Detach();
}

PyObject* BlueClasses::PyGetClassTypes( PyObject* self, PyObject* args )
{
	BlueClasses* pThis = BluePythonCast<BlueClasses*>( self );
	
	const char* modulename = NULL;

	if( !PyArg_ParseTuple(args, "|s", &modulename) )
	{
		return NULL;
	}

	PyObject* list = PyList_New( 0 );
	if( !list )
	{
		return NULL;
	}

	for( ClassReg::iterator i = pThis->m_classes.begin(); i != pThis->m_classes.end(); ++i )
	{
		const Be::ClassRegistration* reg = i->second;
		if( modulename && strcmp(reg->mType->mClassId->GetModule(), modulename) )
		{
			continue;
		}
		PyObject *obj = PyGetTypeInfo( reg->mType );
		if( !obj || PyList_Append( list, obj ) )
		{
			Py_XDECREF( obj );
			Py_DECREF( list );
			return 0;
		}
		Py_DECREF( obj );
	}
	return list;
}

#if BLUE_LIVELIST_ENABLED

PyObject* BlueClasses::PyLiveList( PyObject* self, PyObject* args )
{
	BlueClasses* pThis = BluePythonCast<BlueClasses*>( self );

	if( !PyArg_ParseTuple( args, "" ) )
	{
		return NULL;
	}

	CcpAutoMutex guard( pThis->m_aliveTrackingMutex );

	//copy those, so that counts won't be affected by these operations
	AliveByInst_t bi = pThis->mAliveByInst; 

	// The instance list holds weak references. Manipulating the lists
	// below may trigger a garbage collection, which might in turn cause
	// any of the entries on the list to be freed, leaving us with a
	// dangling reference. To prevent this, we add a strong reference
	// before doing anything else, and clear them at the end.
	for( AliveByInst_i it2 = bi.begin(); it2!= bi.end(); ++it2 )
	{
		it2->first->Lock();
	}

	BluePyList list( 0 );

	for( ClassReg::iterator it = pThis->m_classes.begin(); it != pThis->m_classes.end(); ++it )
	{
		const Be::ClassRegistration* reg = it->second;
		const Be::ClassInfo* clsInfo = reg->mType;
		const Be::Clsid* id = clsInfo->mClassId;

		BluePyTuple line( 5 );
		line.Set( 0, BluePyStr::Format( "%s.%s", id->GetModule(), id->GetName() ) );
		line.Set( 1, BluePy( Py_None, true ) );
		line.Set( 2, BluePyInt( clsInfo->mLiveCount ) );
		line.Set( 3, BluePyInt( clsInfo->mLockCount ) );
		line.Set( 4, BluePyInt( 0 /*it->second.mPyref*/ ) );
		list.Append( line );
	}

	for( AliveByInst_i it2 = bi.begin(); it2!= bi.end(); ++it2 )
	{
		BluePyTuple line( 5 );
		const Be::Clsid *id = it2->first->ClassType()->mClassId;
		line.Set( 0, BluePyStr::Format( "%s.%s", id->GetModule(), id->GetName() ) );
		line.Set( 1, BluePy( BlueWrapObjectForPython( it2->first ) ) );
		line.Set( 2, BluePy( Py_None, true ) );
		line.Set( 3, BluePyInt( it2->second.mLock ) );
		line.Set( 4, BluePyInt( 0 /*it2->second.mPyref*/ ) );
		list.Append(line);
	}

	for( AliveByInst_i it2 = bi.begin(); it2!= bi.end(); ++it2 )
	{
		it2->first->Unlock();
	}

	return list.Detach();
}

#endif


PyObject* BlueClasses::PyLiveCount( PyObject* self, PyObject* args )
{
	BlueClasses* pThis = BluePythonCast<BlueClasses*>( self );

	if( !PyArg_ParseTuple( args, "" ) )
	{
		return NULL;
	}

	BluePyDict d( 0 );

	for( ClassReg::iterator i = pThis->m_classes.begin(); i != pThis->m_classes.end(); ++i )
	{
		const Be::ClassRegistration* reg = i->second;
		const Be::ClassInfo* clsInfo = reg->mType;
		const Be::Clsid* clsid = clsInfo->mClassId;
		int count = clsInfo->mLiveCount;

		BluePyStr s = BluePyStr::Format( "%s.%s", clsid->GetModule(), clsid->GetName() );
		d.Set( s, BluePyInt( count ) );
	}

	return d.Detach();
}

PyObject* BlueClasses::PyLockCount( PyObject* self, PyObject* args )
{
	BlueClasses* pThis = BluePythonCast<BlueClasses*>( self );

	if( !PyArg_ParseTuple( args, "" ) )
	{
		return NULL;
	}

	BluePyDict d( 0 );

	for( auto& i : pThis->m_classes )
	{
		const Be::ClassRegistration* reg = i.second;
		const Be::ClassInfo* clsInfo = reg->mType;
		const Be::Clsid* clsid = clsInfo->mClassId;
		int32_t count = clsInfo->mLockCount;

		BluePyStr s = BluePyStr::Format( "%s.%s", clsid->GetModule(), clsid->GetName() );
		d.Set( s, BluePyInt( count ) );
	}

	return d.Detach();
}

PyObject* BlueClasses::PyGetWrapperList( PyObject* self, PyObject* args )
{
	if( !PyArg_ParseTuple( args, "" ) )
	{
		return NULL;
	}
	return BlueWrapper::GetWrapperList();
}


PyObject* PyCopy( PyObject* self, PyObject* args )
{
	PyObject* srcObj = nullptr;
	if( !PyArg_ParseTuple( args, "O", &srcObj ) )
	{
		return nullptr;
	}

	IRoot* src = BluePythonCast<IRoot*>( srcObj );
	if( !src )
	{
		PyErr_SetString( PyExc_TypeError, "Value is not a Blue object" );
		return nullptr;
	}

	IRootPtr dst;
	if( !BeClasses->CopyTo( src, &dst ) )
	{
		PyErr_SetString( PyExc_RuntimeError, "Couldn't copy object" );
		return nullptr;
	}

	return BlueWrapObjectForPython( dst );
}


PyObject* PyFind( PyObject* self, PyObject* args )
{
	if( !PyTuple_Check( args ) )
	{
		PyErr_SetString( PyExc_TypeError, "Not enough arguments" );
	}

	IRoot* obj = BluePythonCast<IRoot*>(PyTuple_GetItem( args, 0 ) );
	if( !obj )
	{
		PyErr_SetString( PyExc_TypeError, "First argument is not a Blue object" );
		return nullptr;
	}

	PyObject* newArgs = PyTuple_GetSlice( args, 1, PyTuple_Size( args ) );
	PyObject* retValue = PyFindImpl( obj, newArgs );
	Py_DECREF( newArgs );
	return retValue;
}


PyObject* PyTypeInfo( PyObject* self, PyObject* args )
{
	PyObject* srcObj = nullptr;
	if( !PyArg_ParseTuple( args, "O", &srcObj ) )
	{
		return nullptr;
	}

	IRoot* src = BluePythonCast<IRoot*>( srcObj );
	if( !src )
	{
		PyErr_SetString( PyExc_TypeError, "Value is not a Blue object" );
		return nullptr;
	}

	return PyGetTypeInfo( src->ClassType(), src->GetFlags() );
}

#endif

const Be::ClassInfo* BlueClasses::ExposeToBlue()
{
	EXPOSURE_BEGIN( BlueClasses, "BlueClasses handles class registration" )
		MAP_INTERFACE( IBlueClasses )

		MAP_METHOD
		(
			"GetClassTypes",
			PyGetClassTypes,
			"Get a list of all registered classes\n"
			":param moduleName: name of the module; if ommited, returns classes from all modules\n"
			":type moduleName: Optional[str]\n"
			":rtype: list"
		)

		MAP_METHOD
		(
			"CreateInstance",
			PyCreateInstance,
			"Creates an instance of the given class\n"
			":param clsid: class id\n"
			":type clsid: str\n"
			":param args: __init__ call arguments\n"
			":type args: Optional[Any]\n"
			":rtype: IRoot"
		)

		MAP_METHOD
		(
			"LiveCount",
			PyLiveCount,
			"Returns a number of live instances for each blue-exposed type in a dict with keys being type names.\n"
			":rtype: dict[str, int]"
		)

		MAP_METHOD(
			"LockCount",
			PyLockCount,
			"Returns a number of locks for each blue-exposed type in a dict with keys being type names. Only works in debug builds\n"
			":rtype: dict[str, int]" )

		MAP_METHOD(
			"GetWrapperList",
			PyGetWrapperList,
			"Returns a list of all blue objects exposed to Python.\n"
			":rtype: list[IRoot]" )

#if BLUE_LIVELIST_ENABLED
		MAP_METHOD
		(
			"LiveList",
			PyLiveList,
			"Returns a list with stats on live objects.\n"
			":rtype: list"
		)
#endif
		MAP_METHOD_AND_WRAP
		(
			"ProcessAllPendingDeletes",
			ProcessAllPendingDeletes,
			"Processes all pending object deletes, with no time limit."
		)

		MAP_ATTRIBUTE
		(
			"maxTimeForPendingDeletes",
			m_maxTimeForPendingDeletes,
			"Maximum time, in milliseconds, that is spent per frame in processing\n"
			"pending deletes. Setting this value higher may reduce memory spikes\n"
			"at the cost of framerate spikes.",
			Be::READWRITE
		)

		MAP_ATTRIBUTE
		(
			"maxPendingDeletes",
			m_maxPendingDeletes,
			"Maximum number of pending deletes. Setting this value higher may\n"
			"reduce framerate spikes at the cost of memory spikes.",
			Be::READWRITE
		)

		MAP_PROPERTY
		(
			"pendingDeletesEnabled",
			IsPendingDeletesEnabled, SetPendingDeletesEnabled,
			"If set, then Blue objects are not deleted immediately, but processed\n"
			"at the end of the frame. The maximum time spent on deleting objects\n"
			"can be limited, spreading spikes over multiple frames."
		)

		MAP_METHOD
		(
			"Copy",
			PyCopy,
			"Copy the given object\n"
			":param src: source object\n"
			":type src: IRoot\n"
			":rtype: IRoot"
		)

		MAP_METHOD
		(
			"Find",
			PyFind,
			"Find a Blue class type, or list of path objects to it\n"
			":param obj: root object\n"
			":type obj: IRoot\n"
			":param className: class name or list of class names, e.g. ['trinity.TriEffect']\n"
			":type className: str | list[str]\n"
			":param maxLevel: maximum depth to search to (default -1)\n"
			":type maxLevel: Optional[int]\n"
			":param prune: prune duplicate instances (default False)\n"
			":type prune: Optional[bool]\n"
			":param nParents: number of parents to return in a list (default -1)\n"
			":type nParents: Optional[int]\n"
			":rtype: list[IRoot]"
		)

		MAP_METHOD
		(
			"GetTypeInfo",
			PyTypeInfo,
			"Returns various type info for the given object\n"
			":param obj: object\n"
			":type obj: IRoot\n"
		)

		MAP_METHOD_AND_WRAP
		(
			"GetPersistedClassesReport",
			GetPersistedClassesReport,
			"Write the table of classes known to a string, listing all persisted members\n"
			"and their type for each class."
		)
	EXPOSURE_END()
}
