// Copyright © 2026 CCP ehf.

#pragma once
#ifndef Find_h
#define Find_h

#include "IList.h"
#include "IBlueDict.h"
#include "BlueUtil.h"
#include "BlueTypes.h"
#include "BlueSmartPy.h"

class RouteStep
{
public:
    enum StepType
    {
        ATTRIBUTE,
        INDEX,
        KEY
    };
    struct AttributeRef
    {
        const Be::ClassInfo* type;
        const Be::VarEntry* entry;
        ptrdiff_t offset;
    };
    struct StepValue
    {
        std::string key;
        ssize_t index;
        AttributeRef attribute;
    };

    RouteStep();
    RouteStep( StepType stepType, const StepValue& value, IRoot* root );
    RouteStep( const RouteStep& ref );
    ~RouteStep();

    IRoot* GetNextObject( IRoot* parent=nullptr );
    IRoot* GetStepObject() const;

private:
    StepType m_stepType;
    StepValue m_value;
    IRoot* m_obj;
};

bool FindFirstRoute( IRoot* from, IRoot* to, std::vector<RouteStep>* result );
BluePy PyFindFirstRoute( IRoot* from, IRoot* to );

std::vector<IRootPtr> FindInterface( IRoot* obj, const char* iidName );

PyObject* PyFindImpl( IRoot* pThis, PyObject* args );

BluePy FindRoute( IRoot* from, IRoot* to );
bool FindReference( IRoot* from, IRoot* to );
BluePy FindAllReferences( IRoot* root );


template <typename OnAttribute, typename OnListItem, typename OnDictItem, typename Filter>
void EnumerateChildren( IRoot* obj, const OnAttribute& onAttribute, const OnListItem& onListItem, const OnDictItem& onDictItem, const Filter& filter )
{
	static const Be::IID ListIID = Be::IID( "IList" );
	static const Be::IID DictIID = Be::IID( "IBlueDict" );

	const Be::ClassInfo* type = obj->ClassType();
	// go thru members, see if any is IROOT or IROOTPTR
	for( ssize_t xtraoffs = 0; type; xtraoffs += type->mOffsetToParent, type = type->mParentClassInfo )
	{
		for( const Be::VarEntry* entry = type->mMemberTable; entry->mName; entry++ )
		{
			if( entry->mSize && ( entry->mType == Be::IROOT || entry->mType == Be::IROOTPTR ) && filter( entry ) )
			{
				Be::Var* value = BLUEMAPMEMBEROFFSET( obj, entry, type, xtraoffs );
				IRoot* child;
				if( entry->mType == Be::IROOTPTR )
				{
					child = value->mIRootPtr;
					if( !child )
						continue;
				}
				else
				{ //IRoot, autovar.
					child = reinterpret_cast<IRoot*>( value );
				}
				onAttribute( child, type, entry, xtraoffs );
			}
		}
		for( auto iface = type->mInterfaceTable; iface->mIID; ++iface )
		{
			if( iface->mIID->IsEqual( ListIID ) )
			{
				IList* list = reinterpret_cast<IList*>( reinterpret_cast<uint8_t*>( obj ) + ( iface->mOffset + xtraoffs ) );
				auto [items, size] = list->GetAllItems();
				if( items )
				{
					for( ssize_t i = 0; i < size; i++ )
					{
						if( IRoot* item = items[i] )
						{
							onListItem( item, i );
						}
					}
				}
				else
				{
					for( ssize_t i = 0; i < size; i++ )
					{
						if( IRoot* item = list->GetAt( i ) )
						{
							onListItem( item, i );
						}
					}
				}
			}
			else if( iface->mIID->IsEqual( DictIID ) )
			{
				IBlueDict* dict = reinterpret_cast<IBlueDict*>( reinterpret_cast<uint8_t*>( obj ) + ( iface->mOffset + xtraoffs ) );
				size_t n = dict->GetLength();
				for( size_t i = 0; i < n; ++i )
				{
					const char* key = dict->GetKey( i );
					if( IRoot* item = dict->Subscript( key ) )
					{
						onDictItem( item, i );
					}
				}
			}
		}
	}
}

template <typename OnChild>
void EnumerateChildren( IRoot* obj, const OnChild& onChild )
{
	EnumerateChildren(
		obj,
		[&]( IRoot* child, auto, auto entry, auto ) { onChild( child, entry, 0 ); },
		[&]( IRoot* child, auto index ) { onChild( child, nullptr, index ); },
		[&]( IRoot* child, auto index ) { onChild( child, nullptr, index ); },
		[]( auto ) { return true; } );
}

#endif
