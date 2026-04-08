// Copyright © 2026 CCP ehf.

#include "include/BlueTypes.h"
#include "include/IBlueRtti.h"
#include "include/BlueRegistration.h"
#include "include/BlueListUtil.h"
#include "include/BlueDict.h"
#include "include/BlueMemberIterator.h"
#include "BlueClasses.h"
#include "Copier.h"
#include "BluePyWrap.h"
// #include "include/CcpStatistics.h"

// CCP_STATS_DECLARE( pendingDeletes, "Blue/Classes/PendingDeletes", true, CST_COUNTER_LOW, "Number of pending object deletes per frame" );
// CCP_STATS_DECLARE( requestedDeletes, "Blue/Classes/RequestedDeletes", true, CST_COUNTER_LOW, "Number of requested object deletes per frame" );
// CCP_STATS_DECLARE( performedDeletes, "Blue/Classes/PerformedDeletes", true, CST_COUNTER_LOW, "Number of objects deleted per frame" );
// CCP_STATS_DECLARE( performedCreates, "Blue/Classes/PerformedCreates", true, CST_COUNTER_LOW, "Number of object created per frame" );

typedef std::list<IRoot*> PendingDeleteList;

// The global instances of IBlueClasses. This gets initialized automatically below
// with the BeClassesInitializer.
BLUEIMPORT IBlueClasses* BeClasses = nullptr;
BLUE_REGISTER_GLOBAL_AS_MODULE_OBJECT( "classes", BeClasses );

BLUEIMPORT const Be::IID BlueDefaultIID( "Default" );
BLUEIMPORT const Be::IID BlueRotationIID( "Rotation" );
BLUEIMPORT const Be::IID BlueDirectionIID( "Direction" );
BLUEIMPORT const Be::IID BlueColorIID( "Color" );
BLUEIMPORT const Be::IID BlueMatrixIID( "Matrix" );
BLUEIMPORT const Be::IID BlueTimeIID( "Be::Time" );

namespace
{
	// BlueClasses has a private constructor to prevent multiple instances of it,
	// and has an Initialize function to create the one instance we want. This
	// helper class constructs that instance and sets the BeClasses global variable.
	class BeClassesInitializer
	{
	public:
		BeClassesInitializer()
		{
			BlueClasses::Initialize();
		}
	};

	BeClassesInitializer doTheInit;

	CcpThreadId_t s_mainThreadId = 0;

	bool s_useDelayedDeletes = false;

	PendingDeleteList& GetPendingDeleteList()
	{
		static PendingDeleteList s_pendingDeleteList;
		return s_pendingDeleteList;
	}

	// Blue class rtti.  We compile a list of attribute and pymethods on
	// demand.


	//The key is a pointer to a static string.
	typedef std::string BlueRttiKey;

	typedef std::unordered_map<BlueRttiKey, BlueRttiValue> BlueRttiMap;
	struct BlueRttiType : public IBlueRtti
	{
		BlueRttiType( const Be::ClassInfo* ci )
		{
			Init( ci );
		}

		BlueRttiMap		mVarMap;  //contains only blue variables
		BlueRttiMap		mFullMap; //both

		void Init( const Be::ClassInfo *ci )
		{
			//populate the map.  The resolution order for GetAttr is normally:
			// 1) blue attributes
			// 2) Python methods
			// 3) generic python thunkers
			//We therefore find them all in this order and then, insert them
			//in reverse into the map, so that earlier ones override the later.
			//(that is, derived classes override inherited classes)

			typedef std::vector<std::pair<BlueRttiKey, BlueRttiValue> > tmp_t;
			tmp_t tmp;

			//Attributes:
			ssize_t xtraoffset = 0;  //offset because of parent classes
			//finaloffset is the difference between an IRoot pointer to the the final class, and its
			//implementation.  Really, it should be 0 or stuff will crash left and right.
			ssize_t finaloffset = ci->mInterfaceTable[0].mOffset;

			const Be::ClassInfo *type;
			for (type = ci; type; xtraoffset += type->mOffsetToParent, type = type->mParentClassInfo)
			{
				for (const Be::VarEntry* entry = type->mMemberTable; entry->mName; entry++)
				{
					if (entry->mEditFlags & Be::HIDDEN)
						continue; //This is a hidden property

					BlueRttiValue val;
					if( entry->mGetProperty )
					{
						val.mType = BlueRttiValue::pyproperty;
						val.mPyProperty = entry;
					}
					else
					{
						val.mType = BlueRttiValue::var;
						val.mVar = entry;
						val.mXtraOffset = xtraoffset;
						val.mTotalOffset = entry->mOffset + xtraoffset - finaloffset;
					}
					tmp.push_back(tmp_t::value_type(entry->mName, val));
					if (type->mInterfaceTable[0].mOffset)
					{
						CCP_ASSERT(0);
					}
				}
			}

			//Python methods
			for (type = ci; type; type = type->mParentClassInfo)
			{
				for (
					const BlueMethodDefinition* method = type->mPyMethodTable; 
					method->ml_name;
					method++
					)
				{
					BlueRttiValue val;
					val.mType = BlueRttiValue::pymethod;
					val.mPyMethod = method;
					tmp.push_back(tmp_t::value_type(method->ml_name, val));
				}
			}

			//Thunkers:
			std::vector< Be::IID> interfaces;
			for (type = ci; type; type = type->mParentClassInfo)
			{
				for (
					const Be::InterfaceEntry* entry = type->mInterfaceTable;
					entry->mIID;
				entry++
					)
				{
					interfaces.push_back(*entry->mIID);
				}
			}

			for( size_t i = 0; i < interfaces.size(); ++i ) 
			{
				ThunkerRegsVector& regs = BlueRegistration::GetGlobalThunkerRegs();
				for( ThunkerRegsVector::const_iterator it = regs.begin(); it != regs.end(); ++it )
				{
					auto defs = it->first;
					const Be::IID& iid = it->second;
					if( iid.IsEqual( interfaces[i] ) )
					{
						auto method = defs;
						for(; method->ml_name; method++)
						{
							BlueRttiValue val;
							val.mType = BlueRttiValue::pymethod;
							val.mPyMethod = method;
							tmp.push_back(tmp_t::value_type(method->ml_name, val));
						}
					}
				}
			}

			//now, create the map by backwards traversal.
			//Actually, work forward and use the map.insert method, that
			//won't override an entry that's already there
			for(tmp_t::iterator i = tmp.begin(); i != tmp.end(); ++i)
			{
				mFullMap.insert(*i);
				if (i->second.mType == BlueRttiValue::var  || i->second.mType == BlueRttiValue::pyproperty)
					mVarMap.insert(*i);
			}
		}

#if BLUE_WITH_PYTHON
		PyObject* GetMethodsAsList()
		{
			PyObject *list = PyList_New(0);
			
			for( BlueRttiMap::const_iterator i = mFullMap.begin(); i != mFullMap.end(); ++i )
			{
				if( i->second.mType != BlueRttiValue::pymethod )
				{
					continue;
				}
				PyObject *name = PyString_FromString( i->second.mPyMethod->ml_name );
				PyList_Append(list, name);
				Py_DECREF(name);
			}
			return list;
		}
#endif

		BlueRttiValue* FindAttribute( const char* name )
		{
			BlueRttiMap::iterator foundIt = mVarMap.find( name );
			if( foundIt != mVarMap.end() )
			{
				return &foundIt->second;
			}

			return nullptr;
		}

		BlueRttiValue* FindMethod( const char* name )
		{
			BlueRttiMap::iterator foundIt = mFullMap.find( name );
			if( foundIt != mFullMap.end() )
			{
				return &foundIt->second;
			}

			return nullptr;
		}

	};

}


BlueClasses::BlueClasses( IRoot* lockobj /*= NULL */ ) :
	mGenericThunkers( "BluePyOS/mGenericThunkers" ),
	m_classes( "BlueClasses/m_classes" ),
	m_classesByName( "BlueClasses/m_classesByName"),
	m_maxTimeForPendingDeletes( 1.0f ),
	m_maxPendingDeletes( 10000 )
#if BLUE_LIVELIST_ENABLED
	,
	mAliveByInst( "BlueClasses/mAliveByInst" ),
	m_aliveTrackingMutex( "BlueClasses", "m_aliveTrackingMutex" )
#endif
{

}

BlueClasses::~BlueClasses()
{

}

bool BlueClasses::Initialize()
{
	static CBlueClasses instance;
	BeClasses = &instance;

	s_mainThreadId = CcpGetCurrentThreadId();
	return true;
}

void BlueClasses::RegisterSingleClass( const Be::ClassRegistration* cr )
{
	const Be::Clsid* toBeAdded = cr->mType->mClassId;

	// Assert that a class with same module and name hasn't been registered already!
	Be::Clsid clsid;
	if( FindClsid( clsid, toBeAdded->GetModule(), toBeAdded->GetName() ) )
	{
		CCP_LOGERR( "Class %s.%s is already registered!", toBeAdded->GetModule(), toBeAdded->GetName() );
		return;
	}

	m_classes.insert( ClassReg::value_type( toBeAdded, cr ) );
	m_classesByName.insert( ClassNameReg::value_type( toBeAdded->GetName(), cr ));
}

void BlueClasses::UnregisterSingleClass( const Be::ClassRegistration* cr )
{
	const Be::Clsid* toBeRemoved = cr->mType->mClassId;

	// Assert that the class has been registered already
	Be::Clsid clsid;
	CCP_ASSERT_M( 
		FindClsid( clsid, toBeRemoved->GetModule(), toBeRemoved->GetName() ), 
		"Class name is not registered!" );

	m_classesByName.erase( toBeRemoved->GetName() );
	m_classes.erase( toBeRemoved );
}

void BlueClasses::RegisterClasses( const Be::ClassRegistration table[] )
{
	for( const Be::ClassRegistration *i = &table[0]; i->mType; ++i )
	{
		RegisterSingleClass( i );
	}
}


void BlueClasses::UnregisterClasses( const Be::ClassRegistration table[] )
{
	for( const Be::ClassRegistration *i = &table[0]; i->mType; ++i )
	{
		UnregisterSingleClass( i );
	}
}


void BlueClasses::RegisterClasses( std::vector<Be::ClassRegistration>& table )
{
	for( std::vector<Be::ClassRegistration>::iterator it = table.begin(); it != table.end(); ++it )
	{
		RegisterSingleClass( &(*it) );
	}
}


void BlueClasses::UnregisterClasses( std::vector<Be::ClassRegistration>& table )
{
	for( std::vector<Be::ClassRegistration>::iterator it = table.begin(); it != table.end(); ++it )
	{
		UnregisterSingleClass( &(*it) );
	}
}

const Be::ClassRegistration* BlueClasses::GetClassRegistration( const Be::Clsid& clsid )
{
	const Be::ClassRegistration* reg = GetClassRegImpl( clsid );

	return reg;
}

bool BlueClasses::FindClsid( Be::Clsid& clsid, const char* module, const char* name )
{
	// Note that we don't allow name clashes between modules. This is primarily
	// due to a shortcoming in the .red files - if we extend them to allow for
	// module names when storing objects we can put the support for module
	// names back in.

	ClassNameRegIt it = m_classesByName.find( name );
	if( it != m_classesByName.end() )
	{
		clsid = *it->second->mType->mClassId;
		return true;
	}

	return false;
}

const Be::Clsid* BlueClasses::FindClsid( const char* name )
{
	ClassNameRegIt it = m_classesByName.find( name );
	if( it != m_classesByName.end() )
	{
		return it->second->mType->mClassId;
	}

	return nullptr;
}

const Be::ClassRegistration* BlueClasses::GetClassRegImpl( const Be::Clsid& clsid )
{
	ClassReg::iterator i = m_classes.find( &clsid );
	if( i != m_classes.end() )
	{
		return i->second;
	}
	return 0;
}

bool BlueClasses::QueryThisInterface( 
	void* pThis, 
	const Be::IID& riid, 
	void** ppv, 
	const Be::ClassInfo* type, 
	BLUEQIOPT options )
{
	const Be::InterfaceEntry* entry;

	const Be::ClassInfo* tmptype = type;	
	ssize_t xtraoffs = 0;

	for( ; type; xtraoffs += type->mOffsetToParent, type = type->mParentClassInfo )
	{
		for (entry = type->mInterfaceTable; entry->mIID; entry++)
		{
			if (entry->mIID->IsEqual(riid))
			{
				*ppv = (char*)pThis + (entry->mOffset + xtraoffs);
				(*(IRoot**)ppv)->Lock();

				return true;
			}
		}
	}

	*ppv = NULL;

	if( tmptype && (options & BEQI_SILENT) == 0 )
	{
		CCP_LOGERR( "Interface %s not found on class %s.%s\n",
			riid.GetName(),
			tmptype->mClassId->GetModule(),
			tmptype->mClassId->GetName()
			);
	}

	return false;
}


const Be::VarEntry* BlueClasses::FindVariable( const char* name, const Be::ClassInfo* type, ptrdiff_t* xtraoffset )
{
	BlueRttiValue* val = GetRtti( type )->FindAttribute( name );
	if( val )
	{
		*xtraoffset = val->mXtraOffset;
		return val->mVar;
	}
	
	return nullptr;
}

bool BlueClasses::CreateInstance( const Be::Clsid& clsid, const Be::IID& riid, void** ppv )
{	
	*ppv = NULL;
	const Be::ClassRegistration* reg = GetClassRegistration(clsid);

	if( !reg )
	{
		return false;
	}

	if( reg->mCreateFn( riid, ppv ) )
	{
		return true;
	}

	return false;
}


bool BlueClasses::CreateInstanceFromName( const char* className, const Be::IID& riid, void** ppv )
{
	ClassNameReg::const_iterator it = m_classesByName.find( className );
	if( it == m_classesByName.end() )
	{
		return false;
	}
	
	const Be::ClassRegistration* reg = it->second;
	
	if( reg->mCreateFn( riid, ppv ) )
	{
		return true;
	}

	return false;
}

void BlueClasses::UpdateObjectCount( IRoot* obj, int instinc, int lockinc )
{
	const Be::ClassInfo* clsInfo = obj->ClassType();
	clsInfo->mLiveCount += instinc;
	clsInfo->mLockCount += lockinc;

#if BLUE_LIVELIST_ENABLED
	CcpAutoMutex guard( m_aliveTrackingMutex );

	if (instinc >= 0) {
		std::pair<AliveByInst_i, bool> res2 =
			mAliveByInst.insert(AliveByInst_t::value_type(obj, AliveInstEntry()));

		CCP_ASSERT( (res2.second && instinc == 1) || (!res2.second && instinc == 0) );

		res2.first->second.mLock += lockinc;
	}
	else
	{
		AliveByInst_i it = mAliveByInst.find( obj );
		if( it != mAliveByInst.end() )
		{
			mAliveByInst.erase( it );
		}
		else
		{
			CCP_LOGERR( "Attempting to delete an object that has already been deleted" );
		}
	}
#endif
}

bool BlueClasses::CopyTo( 
	IRoot* source, 
	IRoot** dest, 
	ICopier::CopyOverrideCallback copyOverride, 
	void* overrideContext,
	ICopier::PostCopyCallback postCopy,
	void* postCopyContext )
{
	CCopier copier;
	copier.SetCopyOverrideCallback( copyOverride, overrideContext );
	copier.SetPostCopyCallback( postCopy, postCopyContext );
	return copier.CopyTo(source, dest);
}

bool BlueClasses::CloneTo( IRoot* source, IRoot** dest )
{
	CCopier copier;
	return copier.CloneTo(source, dest);
}

void BlueClasses::ProcessPendingDeletes()
{
	// CCP_STATS_ZONE( __FUNCTION__ );

	if( m_maxTimeForPendingDeletes <= 0.0f )
	{
		ProcessAllPendingDeletes();
	}

	BeTimer timer;

	float maxTimeInSec = m_maxTimeForPendingDeletes / 1000.0f;

	PendingDeleteList& pendingDeletes = GetPendingDeleteList();
	while( !pendingDeletes.empty() )
	{
		IRoot* obj = pendingDeletes.front();
		pendingDeletes.pop_front();

		obj->FinalDelete();

		if( (pendingDeletes.size() < m_maxPendingDeletes) && (timer.GetSeconds() > maxTimeInSec) )
		{
			// Break out of the loop if we're out of time and we're under the maximum allowed.
			break;
		}
	}

	// CCP_STATS_SET( pendingDeletes, pendingDeletes.size() );
}

void BlueClasses::ProcessAllPendingDeletes()
{
	// CCP_STATS_ZONE( __FUNCTION__ );

	PendingDeleteList& pendingDeletes = GetPendingDeleteList();
	while( !pendingDeletes.empty() )
	{
		IRoot* obj = pendingDeletes.front();
		pendingDeletes.pop_front();

		obj->FinalDelete();
	}
}

void BlueClasses::SetPendingDeletesEnabled( bool b )
{
	s_useDelayedDeletes = b;
}

bool BlueClasses::IsPendingDeletesEnabled() const
{
	return s_useDelayedDeletes;
}


void BlueClasses::RegisterThunker(
	const BlueMethodDefinition* defs,
	const Be::IID& riid
	)
{
	GenericThunker thunk;
	thunk.mDefs = defs;
	thunk.mIID = &riid;

	mGenericThunkers.push_back(thunk);
}

IBlueRtti* BlueClasses::GetRtti( const Be::ClassInfo *ci )
{
	if( !ci->mRtti )
	{
		ci->mRtti = CCP_NEW( "rtti" ) BlueRttiType( ci );
	}
	return ci->mRtti;
}


static bool CompareClsidNames( const Be::ClassInfo* cls1, const Be::ClassInfo* cls2 )
{
	if( strcmp( cls1->mClassId->GetName(), cls2->mClassId->GetName() ) < 0 )
	{
		return true;
	}

	return false;
}


std::string BlueClasses::GetPersistedClassesReport()
{
	std::string result;

	char buffer[512];
	std::list<const Be::ClassInfo*> classes;

	for( auto it = m_classesByName.begin(); it != m_classesByName.end(); ++it )
	{
		const Be::ClassInfo* clsInfo = it->second->mType;

		BlueMemberIterator memberIt( clsInfo );
		if( !memberIt.Eof() )
		{
			classes.push_back( clsInfo );
		}
	}

	classes.sort( CompareClsidNames );

	for( auto it = classes.begin(); it != classes.end(); ++it )
	{
		const Be::ClassInfo* clsInfo = *it;

		BlueMemberIterator memberIt( clsInfo );
		if( !memberIt.Eof() )
		{
			sprintf_s( buffer, "%s:\n", clsInfo->mClassId->GetName() );
			result += buffer;

			for(; !memberIt.Eof(); memberIt.Next() )
			{
				const Be::VarEntry * const entry = memberIt.Entry();

				sprintf_s( buffer, "-   %s:\n", entry->mName );
				result += buffer;

				sprintf_s( buffer, "    -   %d\n", entry->mType );
				result += buffer;

				switch( entry->mType )
				{
                case Be::FLOATARRAY:
					{
						long long memberCount = static_cast<long long>( entry->GetFloatArraySize() );
						sprintf_s( buffer, "    -   %lld\n", memberCount );
						result += buffer;
					}
					break;
                case Be::DOUBLEARRAY:
					{
						long long memberCount = static_cast<long long>( entry->GetDoubleArraySize() );
						sprintf_s( buffer, "    -   %lld\n", memberCount );
						result += buffer;
					}
					break;
                case Be::INTARRAY:
					{
						long long memberCount = static_cast<long long>( entry->GetIntArraySize() );
						sprintf_s( buffer, "    -   %lld\n", memberCount );
						result += buffer;
					}
					break;
                default:
                    break;
				}
			}
		}
	}

	return result;
}

void* BlueInternalCreate( size_t size, const char* name )
{
	// CCP_STATS_INC( performedCreates );

	char* tmp;

	size += sizeof( BlueLockData );

	tmp = (char*)CCP_MALLOC( name, size );

	if( !tmp )
	{
		return nullptr;
	}

	reinterpret_cast<BlueLockData*>( tmp )->mPythonKlass = NULL;
	reinterpret_cast<BlueLockData*>( tmp )->mWrapper = NULL;
	tmp += sizeof (BlueLockData);


	// The memory block allocated looks something like this:
	// 	[BlueLockData]					[object data]
	//
	// A pointer to [object data] is returned, so some minor trickery
	// takes place when casting this pointer to either type
	return tmp;
}

void BlueInternalFreeObject( void* ptr )
{
	// CCP_STATS_INC( performedDeletes );

	BlueLockData* ld = BlueInternalGetLockData( ptr );

#if BLUE_WITH_PYTHON
	//release deco and wrapper.  One day, these will be a single pointer (a wrapper will keep the deco)
	if (ld->mPythonKlass)
		BlueWrapper::ReleaseDeco(&ld->mPythonKlass);
	ld->mPythonKlass=0;
	if (ld->mWrapper)
		ld->mWrapper->Destroy();
	ld->mWrapper=0;
#endif

	CCP_FREE(ld);
}


// If delayed deletes are enabled, adds 'obj' to the list of pending deletes and
// returns true - otherwise returns false and does nothing.
bool BlueAddPendingDelete( IRoot* obj )
{
	// CCP_STATS_INC( requestedDeletes );

	if( !s_useDelayedDeletes )
	{
		return false;
	}

	if( CcpGetCurrentThreadId() != s_mainThreadId )
	{
		// TODO: Might be safer to do deletes on the main thread. Ideally we can
		// control this per object types - need to add thread safety flags to objects.
		// This check at least prevents corruption of the pending delete list.
		return false;
	}

	GetPendingDeleteList().push_back( obj );

	return true;
}

BlueLockData* BlueInternalGetLockData(void* ptr)
{		
	return (BlueLockData*)((char*)ptr - sizeof (BlueLockData));
};

BlueLockData* BlueInternalHasLockData( IRoot *obj )
{
	if( !(obj->GetFlags() & BLUERT_AUTOVAR ) )
	{
		IRoot* ptr = obj->GetRootObject();
		return BlueInternalGetLockData(ptr);
	}
	return 0;
}
