// Copyright © 2026 CCP ehf.

/* 
	*************************************************************************

	BlueListUtil.h

	Author:    Matthias Gudmundsson
	Created:   Nov. 2000
	OS:        Win32
	Project:   Blue

	Description:   

		Templates and helpers for IBlueList
		Python thunkers for IBlueList


	Dependencies:

		Blue

	(c) CCP 2000

	*************************************************************************
*/

#ifndef _BLUELISTUTIL_H_
#define _BLUELISTUTIL_H_

#include "IList.h"
#include "ICopier.h"
#include "BlueExposureMacros.h"
#include "BlueUtil.h"
#include <CcpCore.h>

#include <vector>
#include <algorithm>
#include <functional>

BLUE_DECLARE( List );

namespace BlueListUtils
{

    template<typename T>
    class HasClsid
    {
        typedef const Be::Clsid* (*Sign)();

        typedef char yes[1];
        typedef char no [2];
        template <typename U, U> struct type_check;
        template <typename _1> static yes &chk(type_check<Sign, &_1::Clsid> *);
        template <typename _1> static no  &chk(...);
    public:
        static bool const value = sizeof(chk<T>(nullptr)) == sizeof(yes);
    };

    template <bool hasClsid>
    struct TypeOptionalClsidHelper
    {
        typedef const Be::Clsid* type;
    };

    template <>
    struct TypeOptionalClsidHelper<false>
    {
    };

    template <typename T>
    static inline typename TypeOptionalClsidHelper<HasClsid<T>::value>::type GetClassClsid()
    {
        return T::Clsid();
    }

    template <typename T>
    static inline typename TypeOptionalClsidHelper<!HasClsid<T>::value>::type GetClassClsid()
    {
        return NULL;
    }

}

class BlueListBase
{
public:
    BLUEIMPORT static const char* mListProps[];

    bool RangeCheck(ssize_t key, size_t size, bool insert, const char* fn,
                    const Be::Clsid* list, const Be::Clsid *elem) const
    {
        if( insert )
        {
            //allow one past end element
            if( key >= -1 && key <= (ssize_t)size )
            {
                return true;
            }
        }
        else
        {
            if( key >= -1 && key < (ssize_t)size )
            {
                return true;
            }
        }

        char elemName[256];
        if( elem )
        {
            sprintf_s(elemName, "%s.%s", elem->GetModule(), elem->GetName() );
        }
        else
        {
            sprintf_s(elemName, "unknown");
        }
        char buffer[512];
        sprintf_s( buffer, "(%s) Key %d out of range, size is %d, by %s", elemName, (int)key, (int)size, fn );
#if BLUE_WITH_PYTHON
        PyErr_SetString( PyExc_IndexError, buffer );
#endif

        return false;
    }

};



template <class T>
struct ListSorter
{
    ListSorter(IList::CompareFn compare, IRoot* context, ssize_t offset) :
            mCompare(compare),
            mContext(context),
            mOffset(offset)
    {
    };

    IList::CompareFn mCompare;
    IRoot* mContext;
    ssize_t mOffset;

    bool operator()(T* x, T* y) const
    {
        return mCompare(
                mContext, (IRoot*)((uintptr_t)x + mOffset), (IRoot*)((uintptr_t)y + mOffset));
    };
};

template <class T>
struct ListSorterT
{
    ListSorterT(IList::CompareFn compare, IRoot* context) :
            mCompare(compare),
            mContext(context)
    {
    };

    IList::CompareFn mCompare;
    IRoot* mContext;

    bool operator()(T* x, T* y) const
    {
        return mCompare(mContext, x->GetRawRoot(), y->GetRawRoot());
    };
};



template <class T, long ops>
class BlueList_Impl :
        public IList,
        public ICopierCustomAssignment,
        private TrackableStdVector<T*>,
        public BlueListBase
{

    typedef BlueList_Impl<T, ops> Class;
    //no assignment or copying
    BlueList_Impl(const Class &other);
    Class &operator= (const Class &other);
public:

    IListNotify* mNotify;

    BlueList_Impl() :
            TrackableStdVector<T*>( "BlueList" ),
            mNotify(NULL),
            mOffset( 0 )
    {
    }

    ~BlueList_Impl()
    {
        //At deletion, no one needs to know we're going away
        mNotify = 0;
        Remove(-1);
    }

    ////////////////////////////////////////////////////////////////////////////
    // The following 'accessors' have been added to support existing code.
    // They are however not perfect because the circumvent the notify mechanism
    // of this list class.  Perhaps that's what was intended.  It is at least
    // to dangerous to change that now. <halldor 2008-05-20>
    using typename TrackableStdVector<T*>::const_iterator;
    using typename TrackableStdVector<T*>::iterator;
    using typename TrackableStdVector<T*>::reverse_iterator;

    typename TrackableStdVector<T*>::const_iterator begin() const { return TrackableStdVector<T*>::begin(); }
    typename TrackableStdVector<T*>::iterator begin() { return TrackableStdVector<T*>::begin(); }
    typename TrackableStdVector<T*>::reverse_iterator rbegin() { return TrackableStdVector<T*>::rbegin(); }
    typename TrackableStdVector<T*>::const_iterator end() const { return TrackableStdVector<T*>::end(); }
    typename TrackableStdVector<T*>::iterator end() { return TrackableStdVector<T*>::end(); }
    typename TrackableStdVector<T*>::reverse_iterator rend() { return TrackableStdVector<T*>::rend(); }
    typename TrackableStdVector<T*>::size_type size() const { return TrackableStdVector<T*>::size(); }
    bool empty() const { return TrackableStdVector<T*>::empty(); }
    T* front() const { return TrackableStdVector<T*>::front(); }
    T* back() const { return TrackableStdVector<T*>::back(); }

    // adding C++0x versions
    typename TrackableStdVector<T*>::const_iterator cbegin() const { return TrackableStdVector<T*>::cbegin(); }
    typename TrackableStdVector<T*>::const_reverse_iterator crbegin() const { return TrackableStdVector<T*>::crbegin(); }
    typename TrackableStdVector<T*>::const_iterator cend() const { return TrackableStdVector<T*>::cend(); }
    typename TrackableStdVector<T*>::const_reverse_iterator rend() const { return TrackableStdVector<T*>::crend(); }

    /////////////////////////////////////////
    // Blue class info

    typedef BlueList_Impl<T, ops> _ClassDef;

    EXPOSE_TO_BLUE()
    {
        EXPOSURE_BEGIN_IMP( _ClassDef, NULL, GetListClsid() )

        MAP_INTERFACE( IList )
        MAP_INTERFACE( ICopierCustomAssignment )

        EXPOSURE_END()
    }

    ssize_t mOffset;


    // syntactic sugar, no range check
    T* operator[](ssize_t key)
    {
        return TrackableStdVector<T*>::at(key);
    }

    // syntactic sugar, no range check
    const T* operator[](ssize_t key) const
    {
        return TrackableStdVector<T*>::at(key);
    }

    // do two things in one function call
    bool Replace(ssize_t key, T* value)
    {
        if (!Remove(key))
            return false;

        return Insert(key, reinterpret_cast<IRoot*>( value ) );
    }

    IRoot* CvIRoot(T* obj) const
    {
        // We know that T is IRoot or derived class so we can simply cast to IRoot by
        // interpreting obj as pointing to IRoot (this is safe because IRoot is always the
        // first inherited class in any branch of the inheritance graph!)
        return reinterpret_cast<IRoot*>(obj);
    }


    //////////////////////////////////////////////////////////////////////////
    // ICopierCustomAssignment
    bool AssignTo(
            ICopierCustomAssignment* other,
            ICopier* copier
    ) override;

    /////////////////////////////////////////
    // IList interface
    void GetInfo(
            ListInfo* info
    ) override
    {
        info->mIID = &BlueInterfaceIID<T>();
        info->mClsid = BlueListUtils::GetClassClsid<T>();
        info->mListOps = ops;
        info->mNotify = mNotify;
    }

    ssize_t GetSize() const override
    {
        return size();
    }


    bool Insert( ssize_t key, IRoot* val ) override;

    bool Append( IRoot* val ) override;

    bool Remove(ssize_t key) override;

    IRoot* GetAt(ssize_t key) const override;

    ssize_t FindKey(IRoot* value, ssize_t keyFrom = 0) override;

    bool Swap(ssize_t key1, ssize_t key2) override;

    void SetNotify(IListNotify* notify) override
    {
        mNotify = notify;
    }

    bool Move(ssize_t from, ssize_t to) override;

    std::pair<IRoot* const*, ssize_t> GetAllItems() const override;

    bool Clear();

    //utility function. explicit assignment function
    void AssignFrom(const Class &other)
    {
        Remove(-1);
        for(size_t i = 0; i<other.size(); i++)
        {
            push_back(other.at(i));
            back()->Lock();
        }
    }
};

template <
        class T,
        long ops = 0
>
class BlueListT : public BlueList_Impl<T, ops>
{
public:

    BlueListT(IRoot* lockobj = NULL)
    {
        BlueList_Impl<T, ops>::mOffset = T::ClassType_()->mInterfaceTable[0].mOffset;
    }

    void Sort(IList::CompareFn compare, IRoot* context)
    {
        std::sort(
                BlueList_Impl<T, ops>::begin(),
                BlueList_Impl<T, ops>::end(),
                ListSorterT<T>(compare, context));
    }
};


template <
        class T,
        long ops = 0
>
class BlueList : public BlueList_Impl<T, ops>
{
public:

    BlueList(IRoot* lockobj = NULL)
    {
        (void)lockobj; // not used
        BlueList_Impl<T, ops>::mOffset = 0;
    }

    void Sort(IList::CompareFn compare, IRoot* context)
    {
        std::sort(
                BlueList_Impl<T, ops>::begin(),
                BlueList_Impl<T, ops>::end(),
                ListSorter<T>(compare, context, BlueList_Impl<T, ops>::mOffset));
    }
};





template <class T>
struct ListSorterC
{
    ListSorterC(IList::CompareFn compare, IRoot* context) :
            mCompare(compare),
            mContext(context)
    {
    };

    IList::CompareFn mCompare;
    IRoot* mContext;

    bool operator()(T& x, T& y) const
    {
        return mCompare(mContext, x.GetRawRoot(), y.GetRawRoot());
    };
};


////////////////////////////////////////////////////////////////////
// Implementation of non-inline template class members.

//////// BlueList_Impl

template <class T, long ops>
bool BlueList_Impl<T, ops>::AssignTo(
        ICopierCustomAssignment* other,
        ICopier* copier
)
{
    _ClassDef* dest = (_ClassDef*)other;

    if( !dest->Remove(-1) )
    {
        return false;
    }

    dest->resize( size() );

    unsigned i;
    for( i = 0; i < size(); i++ )
    {
        IRootPtr tmp;
        bool ok = copier->CopyTo(CvIRoot(TrackableStdVector<T*>::at(i)), &tmp);
        if( ok )
        {
            // Note that do not want to use Replace here, but use the QI for direct
            // assignment into the destination list. This ensures that the OnListModified
            // callbacks only happen after assigning all items.
            if( !tmp->QueryInterface( BlueInterfaceIID<T>(), (void**)&dest->at( i ) ) )
            {
                ok = false;
            }
        }
        if( !ok )
        {
            dest->resize( i );  //don't leave null pointers behind!
            return false;
        }
    }

    if ( i && dest->mNotify	)
    {
        dest->mNotify->OnListModified(BELIST_LOADFINISHED, 0, 0, NULL, dest);
    }

    return true;
}

template <class T, long ops>
bool BlueList_Impl<T, ops>::Insert(ssize_t key, ::IRoot* val)
{
    if( !val )
    {
        return false;
    }

    if( !RangeCheck(key, size(), true, "Insert", Clsid(), BlueListUtils::GetClassClsid<T>()) )
    {
        return false;
    }

    T* tmp;
    if( !val->QueryInterface( BlueInterfaceIID<T>(), (void**)&tmp, BEQI_SILENT ) )
    {
#if BLUE_WITH_PYTHON
        PyErr_SetString( PyExc_TypeError, "Incorrect type" );
#endif
        return false;
    }

    ssize_t rkey = key >=0 ? key : size();

    if( key == -1 )
    {
        this->push_back( tmp );
    }
    else
    {
        this->insert( begin() + key, tmp );
    }

    if( mNotify )
    {
        mNotify->OnListModified(BELIST_INSERTED, rkey, 0, CvIRoot(tmp), this);
    }

    return true;
}


template <class T, long ops>
bool BlueList_Impl<T, ops>::Append( ::IRoot* val )
{
    if( !val )
    {
        return false;
    }

    T* tmp;
    if( !val->QueryInterface( BlueInterfaceIID<T>(), (void**)&tmp, BEQI_SILENT ) )
    {
#if BLUE_WITH_PYTHON
        PyErr_SetString( PyExc_TypeError, "Incorrect type" );
#endif
        return false;
    }

    this->push_back( tmp );

    if( mNotify )
    {
        mNotify->OnListModified( BELIST_INSERTED, size(), 0, val, this);
    }

    return true;
}

template <class T, long ops>
bool BlueList_Impl<T, ops>::Remove(ssize_t key)
{
    if (key == -1)
    {
        if( !size() )
        {
            return true;
        }
        if( mNotify )
        {
            mNotify->OnListModified(BELIST_UNLOADSTART, 0, 0, 0, this);
        }

        while( size() )
        {
            T *pTmp = back();
            TrackableStdVector<T*>::pop_back();
            CvIRoot( pTmp )->Unlock();
        }
    }
    else
    {
        if( !RangeCheck(key, size(), false, "Remove", Clsid(), BlueListUtils::GetClassClsid<T>()) )
        {
            return false;
        }

        typename TrackableStdVector<T*>::iterator i = begin() + key;

        T *pTmp = *i;
        if( pTmp )
        {
            TrackableStdVector<T*>::erase(i);
            if( mNotify )
            {
                mNotify->OnListModified(BELIST_REMOVED, key, 0, CvIRoot(pTmp), this);
            }
            CvIRoot( pTmp )->Unlock();
        }
        else
        {
            TrackableStdVector<T*>::erase(i);
        }
    }

    return true;
}

template <class T, long ops>
IRoot* BlueList_Impl<T, ops>::GetAt(ssize_t key) const
{
    if( !RangeCheck(key, size(), false, "GetAt", Clsid(), BlueListUtils::GetClassClsid<T>()) )
    {
        return NULL;
    }

    return CvIRoot(TrackableStdVector<T*>::at(key));
}

template <class T, long ops>
ssize_t BlueList_Impl<T, ops>::FindKey( ::IRoot* value, ssize_t keyFrom)
{
    if( !value )
    {
        return -1;
    }

    if( keyFrom && !RangeCheck(keyFrom, size(), false, "FindKey", Clsid(), BlueListUtils::GetClassClsid<T>()) )
    {
        return -1;
    }

    T* tmp;
    if( !value->QueryInterface( BlueInterfaceIID<T>(), (void**)&tmp ) )
    {
        return -1;
    }
    value->Unlock();

    for( typename TrackableStdVector<T*>::const_iterator i = TrackableStdVector<T*>::begin() + keyFrom;
         i != TrackableStdVector<T*>::end();
         ++i )
    {
        if( tmp == *i )
        {
            return i - begin();
        }
    }

    return -1;
}

template <class T, long ops>
bool BlueList_Impl<T, ops>::Swap(ssize_t key1, ssize_t key2)
{
    if( !RangeCheck(key1, size(), false, "Swap(a)", Clsid(), BlueListUtils::GetClassClsid<T>()) )
    {
        return false;
    }

    if( !RangeCheck(key2, size(), false, "Swap(b)", Clsid(), BlueListUtils::GetClassClsid<T>()) )
    {
        return false;
    }

    T* tmp = TrackableStdVector<T*>::at(key1);
    TrackableStdVector<T*>::at(key1) = TrackableStdVector<T*>::at(key2);
    TrackableStdVector<T*>::at(key2) = tmp;

    if( mNotify )
    {
        mNotify->OnListModified( BELIST_SWAPPED, key1, key2, NULL, this );
    }
    return true;
}


template <class T, long ops>
bool BlueList_Impl<T, ops>::Move(ssize_t from, ssize_t to)
{
    if( !RangeCheck(from, size(), false, "Move(from)", Clsid(), BlueListUtils::GetClassClsid<T>()) )
    {
        return false;
    }

    if( !RangeCheck(to, size(), false, "Move(to)", Clsid(), BlueListUtils::GetClassClsid<T>()) )
    {
        return false;
    }

    ssize_t i = from;
    T* tmp = TrackableStdVector<T*>::at(from);
    for( ; i < to; i++ )
    {
        TrackableStdVector<T*>::at( i ) = TrackableStdVector<T*>::at( i+1 );
    }
    for( ; i > to; i-- )
    {
        TrackableStdVector<T*>::at( i ) = TrackableStdVector<T*>::at( i-1 );
    }
    TrackableStdVector<T*>::at( to ) = tmp;

    if( mNotify )
    {
        mNotify->OnListModified( BELIST_MOVED, from, to, NULL, this );
    }
    return true;
}

template <class T, long ops>
bool BlueList_Impl<T, ops>::Clear()
{
    return Remove( -1 );
}

template <class T, long ops>
std::pair<IRoot* const*, ssize_t> BlueList_Impl<T, ops>::GetAllItems() const
{
    return {
            reinterpret_cast<IRoot* const*>( TrackableStdVector<T*>::data() ), ssize_t( TrackableStdVector<T*>::size() )
    };
}

#endif
