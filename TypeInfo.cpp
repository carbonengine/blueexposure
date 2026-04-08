// Copyright © 2026 CCP ehf.

#if BLUE_WITH_PYTHON

#include "TypeInfo.h"
#include "BluePyWrap.h"

namespace
{

size_t GetChooserLength( const Be::VarChooser* chooser )
{
	size_t count = 0;
	for( const Be::VarChooser* i = chooser; i->mKey; ++i )
	{
		++count;
	}
	return count;
}

}

PyObject* PyGetChooserInfo( const Be::VarEntry& entry )
{
	const Be::VarChooser* chooser = entry.mChooserTable;

	if( !chooser )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	size_t count = GetChooserLength( chooser );

	PyObject* choosers = PyList_New( count );
	count = 0;

	for( const Be::VarChooser* i = chooser; i->mKey; i++, count++ )
	{
		auto rec = PyList_New( 3 );

		auto tmp = PyString_FromString( i->mKey );
		PyList_SET_ITEM( rec, 0, tmp );

		if( entry.mType == Be::STDSTRING )
		{
			// BlueConvertValueToPython doesn't work properly on choosers for std::string
			tmp = PyString_FromString( "" );
		}
		else
		{
			tmp = BlueConvertValueToPython( &entry, &i->mValue );
		}

		PyList_SET_ITEM( rec, 1, tmp );

		tmp = PyString_FromString( i->mDescription );
		PyList_SET_ITEM( rec, 2, tmp );

		PyList_SET_ITEM( choosers, count, rec );
	}
	return choosers;
}

PyObject* PyGetTypeInfo(const Be::ClassInfo* info, long flags)
{
	// no error checking done - should never fail, and is only used in development

	// Return 4 dicts in a tuple
	// First dict a key-value of the class' type info
	// Second dict a key-value of iids, value is version
	// Third dict a key-tuple of members, tuple is type, iid (name.ver), desc and editflags
	// Fourth dict a key-value of methods, value is doc string
	// create class type info dict
	PyObject* tmp;
	PyObject* dict1 = PyDict_New();

	tmp = PyString_FromString(info->mClassId->GetModule() );
	PyDict_SetItemString(dict1, "module", tmp);
	Py_DECREF(tmp);

	tmp = PyString_FromString(info->mClassId->GetName() );
	PyDict_SetItemString(dict1, "classname", tmp);
	Py_DECREF(tmp);

	tmp = PyString_FromString(info->mDescription ? info->mDescription : "");
	PyDict_SetItemString(dict1, "description", tmp);
	Py_DECREF(tmp);

	tmp = PyInt_FromLong((flags & BLUERT_AUTOVAR) ? 1 : 0);
	PyDict_SetItemString(dict1, "autovar", tmp);
	Py_DECREF(tmp);

	PyObject* signatures = PyDict_New();
	PyDict_SetItemString( dict1, "signatures", signatures );
	Py_DECREF( signatures );

	// create dicts
	PyObject* dict2 = PyDict_New();
	PyObject* dict3 = PyDict_New();
	PyObject* dict4 = PyDict_New();

	for (const Be::ClassInfo* i = info; i; i = i->mParentClassInfo)
	{
		// iterate over iids
		for (const Be::InterfaceEntry* itf = i->mInterfaceTable; itf->mIID; itf++)
		{
			tmp = PyInt_FromLong(0);
			PyDict_SetItemString(dict2, (char*)itf->mIID->GetName(), tmp);
			Py_DECREF(tmp);
		}

		// iterate over members
		for (const Be::VarEntry* var = i->mMemberTable; var->mName; var++)
		{
			PyObject* d = PyDict_New();

			// Type
			tmp = PyInt_FromLong(var->mType);
			PyDict_SetItemString(d, "type", tmp);
			Py_DECREF(tmp);

			// Interface name, if any
			tmp = PyString_FromString(var->mIID ? var->mIID->GetName() : "");
			PyDict_SetItemString(d, "iid_name", tmp);
			Py_DECREF(tmp);

			// Interface version, if any
			tmp = PyInt_FromLong(0);
			PyDict_SetItemString(d, "iid_ver", tmp);
			Py_DECREF(tmp);

			// Description
			tmp = PyString_FromString(var->mDescription ? var->mDescription : "");
			PyDict_SetItemString(d, "description", tmp);
			Py_DECREF(tmp);

			// Edit flags
			tmp = PyInt_FromLong(var->mEditFlags);
			PyDict_SetItemString(d, "editflags", tmp);
			Py_DECREF(tmp);

			// Min/Max values - deprecated
			PyDict_SetItemString(d, "min", Py_None);
			PyDict_SetItemString(d, "max", Py_None);

			// Choosers
			// app.browser.Browse(app.browser.GetSelected().TypeInfo())
			auto choosers = PyGetChooserInfo( *var );
			PyDict_SetItemString( d, "choosers", choosers );
			Py_DECREF( choosers );

			PyDict_SetItemString(dict3, (char*)var->mName, d);
			Py_DECREF(d);
		}

		// iterate over methods
		for (const PyMethodDef* meth = i->mPyMethodTable; meth->ml_name; meth++)
		{
			tmp = PyString_FromString(meth->ml_doc);
			PyDict_SetItemString(dict4, meth->ml_name, tmp);
			Py_DECREF(tmp);
		}
		if( i->mFunctionSignatures )
		{
			for( auto it = i->mFunctionSignatures->begin(); it != i->mFunctionSignatures->end(); ++it )
			{
				PyObject* signature = PyDict_New();
				PyObject* rtype = PyString_FromString( it->second.returnType );
				PyDict_SetItemString( signature, "rtype", rtype );
				Py_DECREF( rtype );
				PyObject* args = PyTuple_New( it->second.argumentCount );
				for( uint32_t j = 0; j < it->second.argumentCount; ++j )
				{
					PyTuple_SET_ITEM( args, j, PyString_FromString( it->second.argumentTypes[j] ) );
				}
				PyDict_SetItemString( signature, "parameters", args );
				Py_DECREF( args );

				PyObject* count = PyInt_FromLong( it->second.optionalCount );
				PyDict_SetItemString( signature, "optionalCount", count );
				Py_DECREF( count );

				PyDict_SetItemString( signatures, it->first.c_str(), signature );
				Py_DECREF( signature );
			}
		}
	}

	PyObject* ret = Py_BuildValue("(OOOO)", dict1, dict2, dict3, dict4);
	Py_DECREF(dict1);
	Py_DECREF(dict2);
	Py_DECREF(dict3);
	Py_DECREF(dict4);

	return ret;
}


#endif
