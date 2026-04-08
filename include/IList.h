// Copyright © 2026 CCP ehf.

#pragma once
#ifndef _ILIST_H_
#define _ILIST_H_

#include "BlueTypes.h"

enum LISTOPS
{
    LIST_NOINSERT	= 0x1,
    LIST_NOREMOVE	= 0x2,
    LIST_NOSWAP		= 0x4,

    LIST_READONLY = LIST_NOINSERT | LIST_NOREMOVE | LIST_NOSWAP,

    LIST_FORCELONG	= 0xFFFFFFFF,
};



struct ListInfo
{
    const Be::IID* mIID;
    const Be::Clsid* mClsid;
    long mListOps;
    struct IListNotify* mNotify;
};


enum BLUELISTEVENT
{
    // event enums
    BELIST_LOADFINISHED	  = 0x06,
    BELIST_UNLOADSTART    = 0x07,
    BELIST_INSERTED		  = 0x08, //after insertion
    BELIST_REMOVED		  = 0x09, //after removal
    BELIST_SWAPPED		  = 0x0a, //after swap
    BELIST_MOVED		  = 0x0c, //after move
    BELIST_EVENTMASK	  = 0x0f,

    // flags
    BELIST_LOADING		  = 0x10,
    BELIST_UNLOADING      = 0x20,
    BELIST_FLAGMASK		  = 0xf0,
};



BLUE_INTERFACE(IListNotify) : public IRoot
{
    virtual void OnListModified(
            long event,		// BLUELISTEVENT values
            ssize_t key,
            ssize_t key2,
            IRoot* value,
            const struct IList* theList
    ) = 0;
};


BLUE_INTERFACE_EXPORT(IList) : public IRoot
{

    //--------------------------------------------------------------------
    // Information
    //--------------------------------------------------------------------

    // Returns length of the controlled sequence - huh? what a load of @!#$
    // this func returns number of elements in container
    virtual ssize_t GetSize() const = 0;


    // Puts some info into 'info'.
    virtual void GetInfo( ListInfo* info ) = 0;


    //--------------------------------------------------------------------
    // Insert, Remove methods
    //--------------------------------------------------------------------

    // If 'key' is -1, the item is added at end of list, else it's added
    // at the given position.
    // 'value' can not be a nullptr, and must be of the appropriate type
    // for the list.
    virtual bool Insert(
            ssize_t key,
            IRoot* val
    ) = 0;


    // If 'key' is out of range, error is submitted.
    // If 'key' is -1, all items are removed from list
    // The function returns 'true' on success.
    virtual bool Remove(
            ssize_t key
    ) = 0;

    // Appends a value to the end of the list. 'val' must not be null.
    virtual bool Append( IRoot* val ) = 0;

    //--------------------------------------------------------------------
    // Access methods - iterations
    //--------------------------------------------------------------------

    // If 'key' falls out of range, NULL is returned and range error
    // is submitted.
    virtual IRoot* GetAt( ssize_t key ) const = 0;

    // Looks for a given value and returns the index of it, or -1 if it's
    // not found. Starts looking from 'keyFrom'. If 'keyFrom' is out of
    // range, the function returns -1 but no specific error is reported.
    virtual ssize_t FindKey(
            IRoot* value,
            ssize_t keyFrom = 0	// by default, start from beginning
    ) = 0;

    //--------------------------------------------------------------------
    // Utility functions
    //--------------------------------------------------------------------

    // Swaps elements 'key1' and 'key2'. Returns false in case of error
    virtual bool Swap(
            ssize_t key1,
            ssize_t key2
    ) = 0;


    // Returns true if 'a' < 'b', false otherwise.
    typedef bool (*CompareFn)(IRoot* context, IRoot* a, IRoot* b);

    // Sorts the list.
    virtual void Sort(
            CompareFn compare,
            IRoot* context
    ) = 0;

    //--------------------------------------------------------------------
    // Notify hook
    //--------------------------------------------------------------------

    // Inserts notify hook.
    virtual void SetNotify(
            IListNotify* notify
    ) = 0;

    //--------------------------------------------------------------------
    // Move - moves an element to a particular position
    //--------------------------------------------------------------------
    virtual bool Move(
            ssize_t from,
            ssize_t to
    ) = 0;

    // Returns a pointer to the first element and list size. Implementations
    // that don't use continious memory for storing items may return (nullptr, size).
    virtual std::pair<IRoot* const*, ssize_t> GetAllItems() const = 0;
};


#endif
