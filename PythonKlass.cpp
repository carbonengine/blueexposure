// Copyright (c) 2026 CCP Games

#if BLUE_WITH_PYTHON
#include "include/PythonKlass.h"
#include "include/BluePythonObject.h"
#include <CcpLog.h>
#include <map>

static CcpLogChannel_t s_ch = CCP_LOG_DEFINE_CHANNEL( "PythonKlass" );

// Implementation of the PythonKlass.  This is a wrapper around an instance of a
// BluePyWrapper from Blue2Py.py

BluePyDict PythonKlass::sClassDict;

// Constructor.  Bind the Blue instance.
// Get a reference to the klass namespace.
// Create a dict with variables mentioned in the namespace.
PythonKlass::PythonKlass(PyObject *klass) :
	mVars(1)
{
	CCP_ASSERT(klass);
	
	//These lists contain variable names.  Init the mVars dict with them.
	//They are set to PyNone to begin with.
	const char* names[] = {"__persistvars__", "__nonpersistvars__"};
	for (int i = 0; i < sizeof names / sizeof names[0]; i++)
	{
		BluePySeq s = BluePy(PyObject_GetAttrString(klass, (char*)names[i]),false);
		if (!s.Check()){
			PyErr_Format(PyExc_TypeError, "%s undeclared or not sequence.", names[i]);
			return;
		}
		//add the variable listed to the dict, with an initial value of None
		Py_ssize_t size = s.Size();
		for( Py_ssize_t j = 0; j < size; j++ )
		{
			mVars.Set( s.Get( j ), Py_None );
		}
	}
	//setting the mKlass, signals success
	mKlass = BluePy(klass,true);
}


// Teardown.
PythonKlass::~PythonKlass()
{
#if 0 // list the stuff we are releaseing
	int pos;
	PyObject* key;
	PyObject* value;

	
	TRACE("Cleaning up mVars\n");

	pos = 0;
	while(PyDict_Next(mVars, &pos, &key, &value)) {
		const char* keystr = PyString_AsString(key);
		const char* valuestr = value->ob_type->tp_name;
		TRACE("Removing %s - %s\n", keystr, valuestr);
	}

#endif
}


//Is the class valid?
bool PythonKlass::ok() const
{
	return !!mKlass;
}


// The guid of the deco class, if defined.  Borrowed reference.
bool PythonKlass::GetGuid(PyObject** guid) const
{
	PyObject *tmp = PyObject_GetAttrString(mKlass, "__guid__");
	if (tmp)
		Py_DECREF(tmp); //borrowed reference
	else
		PyErr_Clear();
	if (guid)
		*guid = tmp;
	return tmp != NULL;
}


//Return a dict with all persistvars
PyObject *PythonKlass::Persist() const
{	
	BluePyDict dict(1);
	
	// This won't fail, it's already been checked in the constructor.
	BluePySeq pv = BluePy(PyObject_GetAttrString(mKlass, "__persistvars__"), false);
	if (!pv) {
		PyErr_Clear();
		//persist everything
		return mVars.NewRef();
	}
	if (!pv.Check())
		return nullptr;
	
	Py_ssize_t size = pv.Size();
	for (Py_ssize_t i = 0; i < size; i++)
		dict.Set(pv.Get(i), mVars.Get(pv.Get(i)));
	return dict.Detach();
}


//sees if the python class is persisted.  It is off by default, but __persistdeco__ can
//be set to 1 do enable it.
bool PythonKlass::IsPersisted(PyObject **guid) const
{
	bool result = IsKlassPersisted(mKlass);
	if (result && guid) {
		if (!GetGuid(guid)) {
			CCP_LOGERR("__guid__ not set for persisting deco class" );
			result = false;
		} else
			Py_XINCREF(*guid);
	}
	return result;
}

//sees if the python class is copied.  It is on by default, but __copytodeco__ can
//be set to 0 do disable it.
bool PythonKlass::IsCopied(PyObject **guid) const
{
	bool result = false;

	PyObject* t = PyObject_GetAttrString(mKlass,"__copytodeco__");
	if(!t) {
		PyErr_Clear();
		result = true;
	}
	else {
	    result = PyObject_IsTrue(t) ? true : false;
	    Py_DECREF(t);
	}
	
	if (result && guid) {
		if (!GetGuid(guid)) {
			CCP_LOGERR("__guid__ not set for copying deco class" );
			result = false;
		} else
			Py_XINCREF(*guid);
	}
	return result;
}


//Create a list of lists of the python attributes of the class.
//The vars and the klass attributes are returned each
PyObject* PythonKlass::GetPythonAttributes()
{
	BluePyTuple retval(2);
	if (!retval)
		return NULL;

	BluePy dir(PyObject_Dir(mKlass), false);
	if (!dir)
		return NULL;
	retval.Set(0, dir);
	retval.Set(1, mVars.Keys());
	return retval.Detach();
}


//Gets an attribute on the deco.  This is either a variable,
//Or an function object bound to the Blue instance.
PyObject* PythonKlass::GetAttr( 
		const char* name, 
		bool* handled,
		PyObject *self	// the self, to with to bind methods
		)
{
	if (*name == '_') {
		// handle special cases
		PyObject *res = 0;
		if (!strcmp(name, "__dict__"))
			// Return the instance dictionary
			res = mVars;
		else if (!strcmp(name, "__class__"))
			// the instance's class
			res = mKlass;
		if (res) {
			Py_INCREF(res);
			*handled = true;
			return res;
		}
	}

	//Try to find a instance variable with this name
	PyObject* attr = PyDict_GetItemString(mVars, (char*)name);
	if (attr)
	{
		Py_INCREF(attr);
		*handled = true;
		return attr;
	}

	// Otherwise, it's a class item.
	attr = PyObject_GetAttrString(mKlass, (char*)name);
	if (attr) {
		*handled = true;
		if (PyCallable_Check(attr)) {
			PyObject *r;
			if (PyMethod_Check(attr))
				// It's a method, bound to a class.  Return it bound to the instance.
				r = PyInstanceMethod_New(attr);
			else
				r = PyMethod_New(attr, self);
			Py_DECREF(attr);
			return r;
		} 
		return attr;//It's just a regular attribute
	} 
	PyErr_Clear();
	*handled = false;
	return NULL;
}


//Gets an attribute on the deco.  This is either a variable,
//Or an function object bound to the Blue instance.
//This is the final try, may use __getattr__ if present
PyObject* PythonKlass::GetAttrFinal( 
		PyObject *self,	// the self, to with to bind methods
		const char* name, 
		bool* handled
		)
{
	// Otherwise, it's a class item.
	*handled = false;
	PyObject *method;
	bool found = LookupMethod(&method, self, "__getattr__");
	if (found) {
		*handled = true;
		if (!method) return 0;
		//Now, call the funcion
		PyObject *r = PyObject_CallFunction(method, const_cast<char*>("(s)"), name);
		Py_DECREF(method);
		return r;
	}
	return 0;
}


//Setting of python attributes.  This deals solely with instance attributes.
int PythonKlass::SetAttr(
    bool *handled,
	PyObject *self, 
	const char* name,
	PyObject* v,
	bool force,
	bool nosetattr
	)
{
	*handled = false;
	if (!force && !nosetattr) { //call this in the noforce case, but only if nosetattr.
		//perform __setattr__ or __delattr__ handling on the deco
		const char *s = v?"__setattr__":"__delattr__";
		PyObject *method;
		bool found = LookupMethod(&method, self, s);
		if (found) {
			*handled = true;
			if (!method) return -1;
		
			//Now, call the funcion
			const char* fmt = v?"(sO)":"(s)";
			PyObject *r = PyObject_CallFunction(method, (char*)fmt, name, v);
			Py_DECREF(method);
			Py_XDECREF(r);
			return r?0:-1;
		}
	}
	
	if (!force && !PyDict_GetItemString(mVars, (char*)name))
		return 0;
	
	*handled = true;
	if (v)
		return PyDict_SetItemString(mVars, (char*)name, v);
	else
		return PyDict_DelItemString(mVars, (char*)name);
}


//Setting of python attributes.  This deals solely with instance attributes.
int PythonKlass::OnSetAttr(
    PyObject *self, 
	const char* name,
	PyObject* v
	)
{
	//perform OnSetAttr or OnDelAttr handling on the deco
	const char *s = v?"OnSetAttr":"OnDelAttr";
	PyObject *method;
	if (LookupMethod(&method, self, s)) {
		if (!method) return -1;
		//Now, call the funcion
		const char* fmt = v?"(sO)":"(s)";
		PyObject *r = PyObject_CallFunction(method, (char*)fmt, name, v);
		Py_DECREF(method);
		Py_XDECREF(r);
		return r?0:-1;
	}
	return 0;
}


PyObject *PythonKlass::Repr()
{
	PyObject *guidObj;
	const char *guidstr;
	if (!GetGuid(&guidObj))
		guidstr = "none";
	else
		guidstr = PyUnicode_AsUTF8(guidObj);

	BluePyStr vars = mVars.Repr();
	if (!vars) return nullptr;

	if (vars.Size() > 160) {
		vars = vars.Slice(0, 160);
		if (!vars) return nullptr;
		vars += BluePyStr("... truncated}");
	}
	return PyUnicode_FromFormat("<deco, guid:%s vars:%s>",
		guidstr, vars.Str());
}


//Traverse all our python members, for Python acyclic GC
int PythonKlass::PyTraverse(visitproc visit, void *arg)
{
	int res = 0; 
	if (mVars)
		res = visit(mVars, arg); 
	if (!res)
		res = visit(mKlass, arg);
	return res;
}


//We are bound to 'source'.  Copy and apply to dest
bool PythonKlass::CopyTo(IRoot const *source, IRoot *dest)
{
	//This is currently pretty lame.  It would be most clean to call
	//A copy to method on the python deco class.  But Alas!
	BluePy guid;
	if (!IsCopied(&guid))
		return true;  //this file is never copied
	
	BluePy members(Persist());
	
	//This currently works by invoking the CreateInstance method in blue to create
	//a class of GUID type.  Would be cleaner just to call some DecoClone routine.
	
	//CreateInstance was put in __builtin__ dict by nasty.py
	BluePy builtins(PyImport_ImportModule("__builtin__"));
	if (!builtins)
		return false;

	//Here we invoke the constructor on the deco object giving an existing
	//blue object and data to initialize it with.
	BluePy pyDest(BlueWrapObjectForPython(dest));
	BluePy dekko(PyObject_CallMethod(
		builtins, 
		const_cast<char*>("CreateInstance"), const_cast<char*>("O(OO)"), (PyObject*)guid, (PyObject*)pyDest, (PyObject*)members ));
	return !!dekko;
}


bool PythonKlass::Create(IRoot *target, Be::Clsid const &clsid, PyObject *members)
{
	BluePyStr PyName = BluePyStr::Format("%s.%s", clsid.GetModule(), clsid.GetName());
	BluePy klass(GetClass(PyName.Str()));
	if (!klass)
		return false;
	
	bool persist = IsKlassPersisted(klass);
	if (!persist) {
		CCP_LOG_CH( s_ch, "Deco class %s isn't persistable, skipped loading", PyName.Str());
		return true;
	}

	if (!PyCallable_Check(klass)) {
		CCP_LOGWARN_CH( s_ch, "Class %s Not callable", PyName.Str());
		return false;
	}
	BluePy PyTarget(BlueWrapObjectForPython(target));
	//call the class constructor.  This was defined in the metaclass.
	BluePy dekko(PyObject_CallFunctionObjArgs(klass, (PyObject*)PyTarget, members, 0));

	return !!dekko;
}


PyObject *PythonKlass::GetClass(const char *name)
{
	if (!sClassDict)
		sClassDict = BluePyDict(1);

	BluePy klass = sClassDict.Get(name);
	if (!klass) {
		// not in our cached dict.  try to find it
		const char *dot = strchr(name, '.');
		BluePy module;
		if (dot) {
			BluePyStr mName(dot-name, name);
			module = BluePy(PyImport_ImportModule((char*)mName.Str()));
		} else {
			module = BluePy(PyImport_ImportModule("__builtin__"));
			dot = name-1;
		}
		if (!module) {
			// todo: PyOS->PyError();
			return 0;
		}

		klass = BluePy(PyObject_GetAttrString(module, (char*) dot+1));
		if (klass)
			sClassDict.Set(name, klass);
		else {
			CCP_LOGWARN_CH( s_ch, "python class %s unknown", name);
			// todo: PyOS->PyError();
		}
	}
	return klass.Detach();
}

bool PythonKlass::IsKlassPersisted(PyObject *klass)
{
	if (!klass)
		return false;
	BluePy persist(PyObject_GetAttrString(klass, "__persistdeco__"));
	if (!persist) {
		PyErr_Clear();
		return false;
	}
	return persist.True();
}


//Look up an instance method
bool PythonKlass::LookupMethod(PyObject **m, PyObject *self, const char *n)
{
	//it turns out that most of the filtering for things like __setattr__ happens on the
	//PyMethod_Check() below.  But presumably it is quicker to do a HasAttrString and GetAttrString
	//rather than have to discard the exception object every time.  Why is there no GetAttrNoExcept?
	
	BluePy method(PyObject_GetAttrString(mKlass, (char*)n));
	if (!method) {
	    PyErr_Clear();
		return false; //method not found
	}
	else if (PyMethod_Check(method.o)) {
		//we must use the method test, or we could get something from mKlass's class, the metaclass.
		// It's a method, bound to a class.  Return it bound to the instance.
		*m = PyMethod_New(PyMethod_Function(method), self);
		return true;
	}
	return false;
}
#endif
