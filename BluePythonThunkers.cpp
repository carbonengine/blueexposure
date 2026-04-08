// Copyright © 2026 CCP ehf.

#if BLUE_WITH_PYTHON

#include "BluePythonThunkers.h"
#include "BluePyWrap.h"
#include "Copier.h"

#include "include/IBlueDict.h"
#include "include/BlueMemberIterator.h"

#include <vector>
#include <stack>
#include <algorithm>
#include "Find.h"
#include "TypeInfo.h"
#include "include/BlueSmartPy.h"

BLUE_REGISTER_THUNKER(IRoot_Thunk::Defs(), IRoot_Thunk::IID());
BLUE_REGISTER_THUNKER(IList_Thunk::Defs(), IList_Thunk::IID());

//////////////////////////////////////////////////////////////////////
//
// IRoot Thunkers
//
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// IRoot::CopyTo
//--------------------------------------------------------------------
PyObject* IRoot_Thunk::PyCopyTo(PyObject* args)
{
	return CloneCopyImpl(args, false);
}

PyObject* IRoot_Thunk::PyCloneTo(PyObject* args)
{
	return CloneCopyImpl(args, true);
}


PyObject* IRoot_Thunk::CloneCopyImpl(PyObject* args, bool clone)
{
	PyObject* pyobj = NULL;
	if (!PyArg_ParseTuple(args, "|O", &pyobj))
		return NULL;
	IRoot* value = NULL;
	if ((pyobj != NULL))
	{
		value = BlueUnwrapObjectFromPython( pyobj );
		if (value == NULL)
		{
			PyErr_SetString( PyExc_TypeError, "Argument must be a blue object");
			return NULL;
		}
	}
	IRootPtr object(value);
	CCopier copier;
	bool result = clone ? copier.CloneTo(this, &object.p) : 
						  copier.CopyTo(this, &object.p);
	if (!result)
		return NULL;
	BluePythonObject* ret =  BlueWrapObjectForPython(object);
	return ret; 
}


//--------------------------------------------------------------------
// IRoot::TypeInfo
//--------------------------------------------------------------------
PyObject* IRoot_Thunk::PyTypeInfo(PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	return PyGetTypeInfo(ClassType(), GetFlags());
}

//--------------------------------------------------------------------
// IRoot::Find
//--------------------------------------------------------------------

PyObject* IRoot_Thunk::PyFind(PyObject* args)
{
	return PyFindImpl( this, args );
}


//--------------------------------------------------------------------
// IRoot::Validate
//--------------------------------------------------------------------


typedef BlueStdSet<IRoot*> RootSet;
typedef RootSet::iterator RootSetIter;
typedef BlueStdVector<const char*> NameStack;
typedef NameStack::iterator NameStackIter;
typedef BlueStdVector<PyObject *> ResList;
typedef ResList::iterator ResListIter;

static void PyValidate_Rec(ResList &res, RootSet &set, NameStack &names, IRoot *obj)
{
	IRoot *key = BlueFinalIRoot(obj);

	//is this pointer in the set?
	if (set.find(key) != set.end()) {
		//DebugBreak();
		// bad, this object has been found.  create a result
		size_t len = names.size() - 1; // dots
		for(NameStackIter it = names.begin(); it != names.end(); ++it)
			len += strlen(*it);
		len += 2;
		char *tmpStr = new char[len];
		char *ptr = tmpStr;
		for(NameStackIter it = names.begin(); it != names.end(); ++it) {
			size_t partlen = strlen(*it);
			strcpy_s(ptr, len, *it);
			ptr += partlen;
			ptr[0] = '.';
			ptr[1] = '\0';
			ptr += 1;
			len -= partlen+1;
		}
		if (ptr > tmpStr)
			ptr[-1] = '\0';	//remove final dot
		
		PyObject *string = PyString_FromString(tmpStr);
		delete[] tmpStr;
		res.push_back(string);
		return;
	}

	//no, it isn't.  Add it to set
	set.insert(key);

	// go thru members, see if any is another blue object
	ssize_t xtraoffs = 0;
	const Be::ClassInfo* type = obj->ClassType();
	for (; type; xtraoffs += type->mOffsetToParent, type = type->mParentClassInfo)
	{
		const Be::VarEntry* entry;
		for (entry = type->mMemberTable; entry->mName; entry++)
		{
			if (entry->mType == Be::IROOTPTR || entry->mType == Be::IROOT)
			{
				Be::Var* value = BLUEMAPMEMBEROFFSET(obj, entry, type, xtraoffs);
				IRoot *child = entry->mType == Be::IROOTPTR ? value->mIRootPtr : reinterpret_cast<IRoot*>( value );
				if (!child)
					continue;
				
				names.push_back(entry->mName);
				PyValidate_Rec(res, set, names, child);
				names.pop_back();
			}
		}
	}
	// am I perhaps a list object?

	IListPtr list( BlueCastPtr( obj ) );
	if (list)
		for (long i = 0; i < list->GetSize(); i++) {
			IRoot *child = list->GetAt(i);
			if (!child)
				continue;
			char arrayName[10];
			sprintf_s(arrayName, "[%ld]", i);
			names.push_back(arrayName);
			PyValidate_Rec(res, set, names, child);
			names.pop_back();
		}
	//remove myself from set
	set.erase(key);
}

PyObject* IRoot_Thunk::PyValidate(PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	NameStack names;
	ResList res;
	RootSet set;

	names.push_back("<operand>");
	PyValidate_Rec(res, set, names, this);

	PyObject *result = 0;
	if (res.size()) {
		//Woohoo, we have a result!
		result = PyList_New(res.size());
		for (unsigned i = 0; i < res.size(); i++)
		{
			PyList_SET_ITEM(result, i, res[i]);
		}
	} else {
		result = Py_None;
		Py_INCREF(result);
	}
	return result;
}


PyObject* IRoot_Thunk::PyGetRefCounts(PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return NULL;
	Py_ssize_t pyc;
	const int bluec = GetRefCount();

	BlueLockData *ld = BlueInternalHasLockData(this);
	if (ld) {
		// The thunker added a single reference when it queried for IRoot
		if (ld->mWrapper)
			pyc = ld->mWrapper->ob_refcnt;
		else
			pyc = -1;
	} else {
		pyc = -1;
	}
	BluePyTuple t(2);
	t.Set(0, BluePy(PyLong_FromLongLong(pyc)));
	t.Set(1, BluePyInt(bluec));
	return t.Detach();
}


//////////////////////////////////////////////////////////////////////
//
// IList Thunkers
//
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// IList::append
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pyappend(PyObject* args)
{
	PyObject* pyobj = NULL;

	if (!PyArg_ParseTuple(args, "|O", &pyobj))
		return NULL;

	ListInfo info;
	GetInfo(&info);

	if (info.mListOps & LIST_NOINSERT)
	{
		PyErr_SetString( PyExc_RuntimeError, "No appends allowed on this list.");
		return NULL;
	}

	IRoot* value = NULL;
	if ((pyobj != NULL))
	{
		value = BlueUnwrapObjectFromPython(pyobj);
		if (value == NULL)
		{
			PyErr_SetString( PyExc_TypeError, "argument must be a blue object");
		return NULL;
		}
	}


	if (!Insert(-1, value))
		return NULL;

	if (!pyobj)
	{
		IRootPtr tmp = GetAt(GetSize() - 1);
		return BlueWrapObjectForPython(tmp);
	}
	else
	{	
		Py_INCREF(Py_None);
		return Py_None;
	}
}


//--------------------------------------------------------------------
// IList::insert
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pyinsert(PyObject* args)
{
	long key;
	PyObject* pyobj = NULL;

	if (!PyArg_ParseTuple(args, "l|O", &key,&pyobj))
		return NULL;

	ListInfo info;
	GetInfo(&info);

	if (info.mListOps & LIST_NOINSERT)
	{
		PyErr_SetString( PyExc_TypeError, "No inserts allowed on this list.");
		return NULL;
	}

	IRoot* value = NULL;
	if ((pyobj != NULL))
	{
		value = BlueUnwrapObjectFromPython(pyobj);
		if (value == NULL)
		{
			PyErr_SetString( PyExc_TypeError, "Argument must be a blue object");
			return NULL;
		}
	}


	if (!Insert(key, value))
		return NULL;
	
	Py_INCREF(Py_None);
	return Py_None;
}


//--------------------------------------------------------------------
// IList::extend
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pyextend(PyObject* args)
{
	PyObject* pyobj;

	if( !PyArg_ParseTuple( args, "O", &pyobj ) )
	{
		return nullptr;
	}

	ListInfo info;
	GetInfo(&info);

	if (info.mListOps & LIST_NOINSERT)
	{
		PyErr_SetString( PyExc_RuntimeError, "No extends allowed on this list.");
		return nullptr;
	}

	//When extending, assume that the source object follows the iterator protocol.
	PyObject *iterator = PyObject_GetIter( pyobj );
	PyObject *item;

	if( !iterator )
	{
		return nullptr;
	}

	while( ( item = PyIter_Next( iterator ) ) )
	{
		IRoot *iroot = BlueUnwrapObjectFromPython(item);
		if (!iroot)
		{
			PyErr_SetString( PyExc_TypeError, "elements must be blue objects");
			return nullptr;
		}

		if( !Insert( -1, iroot ) )
		{
			Py_DECREF( item );
			return nullptr;
		}
		Py_DECREF(item);
	}

	Py_DECREF( iterator );

	if( PyErr_Occurred() )
	{
		return nullptr;
	}

	Py_INCREF(Py_None);
	return Py_None;
}


//--------------------------------------------------------------------
// IList::pop
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pypop(PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	ListInfo info;
	GetInfo(&info);

	if (info.mListOps & LIST_NOREMOVE)
	{
		PyErr_SetString( PyExc_RuntimeError, "No pops allowed on this list.");
		return NULL;
	}

	IRootPtr tmp = GetAt(GetSize() - 1);

	if (!tmp)
		return NULL;

	if (!Remove(GetSize() - 1))
		return NULL;

	return BlueWrapObjectForPython(tmp);
}


//--------------------------------------------------------------------
// IList::remove
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pyremove(PyObject* args)
{
	PyObject* pyobj;

	if (!PyArg_ParseTuple(args, "O", &pyobj))
	{
		return nullptr;
	}

	IRoot* value = BlueUnwrapObjectFromPython(pyobj);
	if (value == NULL)
	{
		PyErr_SetString( PyExc_TypeError, "Item must be blue item.");
		return nullptr;
	}

	ListInfo info;
	GetInfo(&info);

	if (info.mListOps & LIST_NOREMOVE)
	{
		PyErr_SetString( PyExc_RuntimeError, "No removes allowed on this list.");
		return nullptr;
	}

	ssize_t key = FindKey(value);

	if( key == -1 )
	{
		PyErr_SetString( PyExc_ValueError, "Item not found" );
		return nullptr;
	}

	if (!Remove(key))
		return nullptr;

	Py_INCREF(Py_None);
	return Py_None;
}

//--------------------------------------------------------------------
// IList::removeAt
//--------------------------------------------------------------------
PyObject* IList_Thunk::PyremoveAt(PyObject* args)
{
	long key;

	if (!PyArg_ParseTuple(args, "l", &key))
		return NULL;

	ListInfo info;
	GetInfo(&info);

	if (info.mListOps & LIST_NOREMOVE)
	{
		PyErr_SetString( PyExc_RuntimeError, "No removes allowed on this list.");
		return NULL;
	}

	if (!Remove(key))
		return NULL;

	Py_INCREF(Py_None);
	return Py_None;
}

//--------------------------------------------------------------------
// IList::fremove
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pyfremove(PyObject* args)
{
	PyObject* pyobj;

	if (!PyArg_ParseTuple(args, "O", &pyobj))
		return NULL;

	ListInfo info;
	GetInfo(&info);
	if (info.mListOps & LIST_NOREMOVE)
		return PyErr_SetString(PyExc_TypeError, "No removes allowed on this list."), nullptr;
		
	IRoot* value = BlueUnwrapObjectFromPython(pyobj);
	if (!value) {
		//only blue items in the list.  remove failed.
		Py_INCREF(Py_False);
		return Py_False;
	}
	
	ssize_t key = FindKey(value);

	if (key == -1) {
		//return failure
		Py_INCREF(Py_False);
		return Py_False;
	}
	//if it is not the last item, swap it with the last one.
	ssize_t end = GetSize()-1;
	if (key != end)
		Swap(key, end);
	Remove(end);
	Py_INCREF(Py_True);
	return Py_True;
}


//--------------------------------------------------------------------
// IList::index
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pyindex(PyObject* args)
{
	PyObject* pyobj;

	if (!PyArg_ParseTuple(args, "O",  &pyobj))
		return NULL;

	IRoot* value = BlueUnwrapObjectFromPython(pyobj);
	if (value == NULL)
	{
		PyErr_SetString( PyExc_TypeError, "Blue item required");
		return NULL;
	}

	ssize_t key = FindKey(value);

	if( key == -1 )
	{
		PyErr_SetString( PyExc_ValueError, "Item not found" );
		return NULL;
	}

	return PyInt_FromSsize_t(key);
}


//--------------------------------------------------------------------
// IList::count
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pycount(PyObject* args)
{
	PyObject* pyobj;

	if (!PyArg_ParseTuple(args, "O", &pyobj))
		return NULL;
	IRoot* value = BlueUnwrapObjectFromPython(pyobj);
	if (!value)
	{
		PyErr_SetString( PyExc_TypeError, "Blue item required");
		return NULL;
	}

	int count = 0;
	ssize_t key = -1;
	
	while ((key = FindKey(BlueUnwrapObjectFromPython(pyobj), key+1)) >= 0)
		count++;

	return PyInt_FromLong(count);
}


//--------------------------------------------------------------------
// IList::reverse
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pyreverse(PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	ListInfo info;
	GetInfo(&info);

	if (info.mListOps & LIST_NOSWAP)
	{
		PyErr_SetString( PyExc_RuntimeError, "No reverse allowed on this list.");
		return NULL;
	}

	ssize_t i = 0;
	ssize_t j = GetSize();
	
	while (i < j)
	{
		if (!Swap(i, j))
			return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}


//--------------------------------------------------------------------
// IList::sort
//--------------------------------------------------------------------
PyObject* IList_Thunk::Pysort(PyObject*)
{
	return PyErr_SetString( PyExc_RuntimeError, "not implemented yet"), nullptr;
}

namespace
{

bool VarEquals( const Be::VarEntry* entry, const Be::Var* variable, const char* value )
{
	switch( entry->mType )
	{
	case Be::CSTRING:
		if( strcmp( variable->mCharPtr, value ) == 0 )
		{
			return true;
		}
		break;
	case Be::STDSTRING:
		if( strcmp( reinterpret_cast<const std::string*>( variable )->c_str(), value ) == 0 )
		{
			return true;
		}
		break;
	case Be::WCSTRING:
		if( wcscmp( variable->mWCharPtr, CA2W( value ) ) == 0 )
		{
			return true;
		}
		break;
	case Be::STDWSTRING:
		if( wcscmp( reinterpret_cast<const std::wstring*>( variable )->c_str(), CA2W( value ) ) == 0 )
		{
			return true;
		}
		break;
	case Be::SHAREDSTRING:
		if( strcmp( reinterpret_cast<const BlueSharedString*>( variable )->c_str(), value ) == 0 )
		{
			return true;
		}
	case Be::SHAREDSTRINGW:
		if( wcscmp( reinterpret_cast<const BlueSharedStringW*>( variable )->c_str(), CA2W( value ) ) == 0 )
		{
			return true;
		}
	default:
		break;
	}
	return false;
}

}

PyObject* IList_Thunk::PyFindByName(PyObject* args)
{
	const char* name;
	if( !PyArg_ParseTuple( args, "s", &name ) )
	{
		return nullptr;
	}

	auto count = GetSize();
	for( ssize_t i = 0; i < count; ++i )
	{
		auto element = GetAt( i );
		if( !element )
		{
			continue;
		}
		auto type = element->ClassType();
		if( !type )
		{
			continue;
		}

		for( BlueMemberIterator it( element ); !it.Eof(); it.Next() )
		{
			auto entry = it.Entry();
			if( strcmp( entry->mName, "name" ) == 0 )
			{
				if( !entry->mGetProperty && VarEquals( entry, it.Var(), name ) )
				{
					return BlueWrapObjectForPython( element );
				}
				break;
			}
		}
	}

	Py_RETURN_NONE;
}


//--------------------------------------------------------------------
// IList::GetInfo
//--------------------------------------------------------------------
PyObject* IList_Thunk::PyGetInfo(PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return NULL;

	ListInfo info;
	GetInfo(&info);

	PyObject* clsInfo;

	if ( info.mClsid )
		clsInfo = Py_BuildValue("[ss]", info.mClsid->GetModule(), info.mClsid->GetName() );
	else
		clsInfo = Py_BuildValue("[OOO]", Py_None, Py_None, Py_None);

	PyObject* iidInfo;

	if ( info.mIID )
		iidInfo = Py_BuildValue("[s]", info.mIID->GetName() );
	else
		iidInfo = Py_BuildValue("[OO]", Py_None, Py_None );

	PyObject* ret = Py_BuildValue(
		"[NNii]", 
		clsInfo,
		iidInfo, 
		info.mListOps, 0 /* this used to indicate objects in place - not supported anymore. Need to check in jessica if I can simply remove this */
		);

	return ret;
}


namespace
{
	struct BlueModuleInfoPayload
	{
		const Be::ClassRegistration* classRegs;
		size_t classRegsSize;
	};
	typedef std::map<std::string, BlueModuleInfoPayload> BlueModuleInfoMap;
}
static BlueModuleInfoMap s_infoMap;

#if PY_VERSION_HEX >= 0x02070000

PyObject* PyGetClassInfo( PyObject* classRegsPayload, PyObject* args )
{
	std::string modname = (char*)PyCapsule_GetPointer(classRegsPayload, "ClassRegsPayload");
	BlueModuleInfoPayload payload = s_infoMap[modname];
	const Be::ClassRegistration* classRegs = payload.classRegs;
	size_t size = payload.classRegsSize;

	PyObject* classDict = PyDict_New();
	for( size_t i = 0; i < size; ++i )
	{
		const Be::ClassInfo* cInfo = classRegs[i].mType;
		if( cInfo )
		{
			PyObject* classInfoObject = PyGetTypeInfo( cInfo );
			PyDict_SetItemString( classDict, cInfo->mClassId->GetName(), classInfoObject );
			Py_XDECREF( classInfoObject );
		}
	}

	return classDict;
}

void BlueRegisterModuleThunkers( PyObject* module, const Be::ClassRegistration classRegs[], size_t size )
{
	static PyMethodDef thunker = { "GetClassInfo", PyGetClassInfo, METH_NOARGS, 
		"Returns a dict of all classes registered in the module" };

	BlueModuleInfoPayload tmpVal = { classRegs, size };

	s_infoMap[PyModule_GetName( module )] = tmpVal;

	// The dict is a borrowed reference, so no decref later
	PyObject* dict = PyModule_GetDict(module);

	// Register the 'GetClassInfo' with the module
	PyObject* thunk = PyCFunction_New(&thunker, PyCapsule_New((void*)PyModule_GetName(module), "ClassRegsPayload", 0) );
	PyDict_SetItemString(dict, thunker.ml_name, thunk);
	Py_DECREF(thunk);
}

#else

//////////////////////////////////////////////////////////////////////////
// This is for Python 2.6

PyObject* PyGetClassInfo( PyObject* classRegsPayload, PyObject* args )
{
	std::string modname = (char*)PyCObject_AsVoidPtr(classRegsPayload);
	BlueModuleInfoPayload payload = s_infoMap[modname];
	const Be::ClassRegistration* classRegs = payload.classRegs;
	size_t size = payload.classRegsSize;

	PyObject* classDict = PyDict_New();
	for( size_t i = 0; i < size; ++i )
	{
		const Be::ClassInfo* cInfo = classRegs[i].mType;
		if( cInfo )
		{
			PyObject* classInfoObject = PyGetTypeInfo( cInfo );
			PyDict_SetItemString( classDict, cInfo->mClassId->GetName(), classInfoObject );
			Py_XDECREF( classInfoObject );
		}
	}

	return classDict;
}

void BlueRegisterModuleThunkers( PyObject* module, const Be::ClassRegistration classRegs[], size_t size )
{
	static PyMethodDef thunker = { "GetClassInfo", PyGetClassInfo, METH_NOARGS, 
		"Returns a dict of all classes registered in the module" };

	BlueModuleInfoPayload tmpVal = { classRegs, size };

	s_infoMap[PyModule_GetName( module )] = tmpVal;

	// The dict is a borrowed reference, so no decref later
	PyObject* dict = PyModule_GetDict(module);

	// Register the 'GetClassInfo' with the module
	PyObject* thunk = PyCFunction_New(&thunker, PyCObject_FromVoidPtr((void*)PyModule_GetName(module), NULL) );
	PyDict_SetItemString(dict, thunker.ml_name, thunk);
	Py_DECREF(thunk);
}

#endif

#endif
