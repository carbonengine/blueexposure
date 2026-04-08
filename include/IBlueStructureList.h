// Copyright © 2026 CCP ehf.

#ifndef IBlueStructureList_h
#define IBlueStructureList_h

#include "BlueTypes.h"

namespace Be
{
	enum BlueStructureDataType
	{
		DT_INT8			= 0,
		DT_INT16		= 1,
		DT_INT32		= 2,
		DT_FLOAT16		= 3,
		DT_FLOAT32		= 4,
		DT_SHAREDSTRING = 5,
		DT_FLOAT32x4	= 6,
		DT_BOOL8		= 7,

		DT_TYPE_MASK	= 7,

		DT_UNSIGNED_BIT		= 1 << 3,

		DT_SIZE_OFFSET		= 5,
		DT_SIZE_MASK		= 15 << DT_SIZE_OFFSET,
		DT_SIZE_1			= 0 << DT_SIZE_OFFSET,
		DT_SIZE_2			= 1 << DT_SIZE_OFFSET,
		DT_SIZE_3			= 2 << DT_SIZE_OFFSET,
		DT_SIZE_4			= 3 << DT_SIZE_OFFSET,
		DT_SIZE_5			= 4 << DT_SIZE_OFFSET,
		DT_SIZE_6			= 5 << DT_SIZE_OFFSET,
		DT_SIZE_7			= 6 << DT_SIZE_OFFSET,
		DT_SIZE_8			= 7 << DT_SIZE_OFFSET,
		DT_SIZE_9			= 8 << DT_SIZE_OFFSET,
		DT_SIZE_10			= 9 << DT_SIZE_OFFSET,
		DT_SIZE_11			= 10 << DT_SIZE_OFFSET,
		DT_SIZE_12			= 11 << DT_SIZE_OFFSET,
		DT_SIZE_13			= 12 << DT_SIZE_OFFSET,
		DT_SIZE_14			= 13 << DT_SIZE_OFFSET,
		DT_SIZE_15			= 14 << DT_SIZE_OFFSET,
		DT_SIZE_16			= 15 << DT_SIZE_OFFSET,

		BYTE_1			= DT_INT8 | DT_SIZE_1,
		BYTE_2			= DT_INT8 | DT_SIZE_2,
		BYTE_3			= DT_INT8 | DT_SIZE_3,
		BYTE_4			= DT_INT8 | DT_SIZE_4,

		UBYTE_1			= DT_INT8 | DT_SIZE_1 | DT_UNSIGNED_BIT,
		UBYTE_2			= DT_INT8 | DT_SIZE_2 | DT_UNSIGNED_BIT,
		UBYTE_3			= DT_INT8 | DT_SIZE_3 | DT_UNSIGNED_BIT,
		UBYTE_4			= DT_INT8 | DT_SIZE_4 | DT_UNSIGNED_BIT,

		SHORT_1			= DT_INT16 | DT_SIZE_1,
		SHORT_2			= DT_INT16 | DT_SIZE_2,
		SHORT_3			= DT_INT16 | DT_SIZE_3,
		SHORT_4			= DT_INT16 | DT_SIZE_4,

		USHORT_1		= DT_INT16 | DT_SIZE_1 | DT_UNSIGNED_BIT,
		USHORT_2		= DT_INT16 | DT_SIZE_2 | DT_UNSIGNED_BIT,
		USHORT_3		= DT_INT16 | DT_SIZE_3 | DT_UNSIGNED_BIT,
		USHORT_4		= DT_INT16 | DT_SIZE_4 | DT_UNSIGNED_BIT,

		INT32_1			= DT_INT32 | DT_SIZE_1,
		INT32_2			= DT_INT32 | DT_SIZE_2,
		INT32_3			= DT_INT32 | DT_SIZE_3,
		INT32_4			= DT_INT32 | DT_SIZE_4,

		UINT32_1		= DT_INT32 | DT_SIZE_1 | DT_UNSIGNED_BIT,
		UINT32_2		= DT_INT32 | DT_SIZE_2 | DT_UNSIGNED_BIT,
		UINT32_3		= DT_INT32 | DT_SIZE_3 | DT_UNSIGNED_BIT,
		UINT32_4		= DT_INT32 | DT_SIZE_4 | DT_UNSIGNED_BIT,

		FLOAT16_1		= DT_FLOAT16 | DT_SIZE_1,
		FLOAT16_2		= DT_FLOAT16 | DT_SIZE_2,
		FLOAT16_3		= DT_FLOAT16 | DT_SIZE_3,
		FLOAT16_4		= DT_FLOAT16 | DT_SIZE_4,

		UFLOAT16_1		= DT_FLOAT16 | DT_SIZE_1 | DT_UNSIGNED_BIT,
		UFLOAT16_2		= DT_FLOAT16 | DT_SIZE_2 | DT_UNSIGNED_BIT,
		UFLOAT16_3		= DT_FLOAT16 | DT_SIZE_3 | DT_UNSIGNED_BIT,
		UFLOAT16_4		= DT_FLOAT16 | DT_SIZE_4 | DT_UNSIGNED_BIT,

		FLOAT32_1		= DT_FLOAT32 | DT_SIZE_1,
		FLOAT32_2		= DT_FLOAT32 | DT_SIZE_2,
		FLOAT32_3		= DT_FLOAT32 | DT_SIZE_3,
		FLOAT32_4		= DT_FLOAT32 | DT_SIZE_4,


		UFLOAT32_1		= DT_FLOAT32 | DT_SIZE_1 | DT_UNSIGNED_BIT,
		UFLOAT32_2		= DT_FLOAT32 | DT_SIZE_2 | DT_UNSIGNED_BIT,
		UFLOAT32_3		= DT_FLOAT32 | DT_SIZE_3 | DT_UNSIGNED_BIT,
		UFLOAT32_4		= DT_FLOAT32 | DT_SIZE_4 | DT_UNSIGNED_BIT,

		BOOL8_1			= DT_BOOL8 | DT_SIZE_1,
		BOOL8_2			= DT_BOOL8 | DT_SIZE_2,
		BOOL8_3			= DT_BOOL8 | DT_SIZE_3,
		BOOL8_4			= DT_BOOL8 | DT_SIZE_4,

		SHAREDSTRING_1	= DT_SHAREDSTRING | DT_SIZE_1,
		SHAREDSTRING_2	= DT_SHAREDSTRING | DT_SIZE_2,
		SHAREDSTRING_3	= DT_SHAREDSTRING | DT_SIZE_3,
		SHAREDSTRING_4	= DT_SHAREDSTRING | DT_SIZE_4,

		MATRIX32_4x4	= DT_FLOAT32x4 | DT_SIZE_4,

		DT_UNKNOWN_TYPE	= 0xffFFffFFu
	};
}

struct BlueStructureDefinition
{
	const char* m_name;
	Be::BlueStructureDataType m_dataType;
	uint32_t m_offset;
	Be::VarChooser* m_chooser;
};

BLUE_DECLARE_INTERFACE_EXPORT( IBlueStructureList );

struct IBlueStructureListNotify
{
	enum Event
	{
		BLUE_STRUCTURE_LIST_INSERTED,
		BLUE_STRUCTURE_LIST_REMOVED,
		BLUE_STRUCTURE_LIST_CLEARED,
		BLUE_STRUCTURE_LIST_ITEM_CHANGED,
	};

	virtual void OnStructureListModified( Event event, const void* item, size_t index, IBlueStructureList* list ) = 0;
};


BLUE_INTERFACE_EXPORT( IBlueStructureList ) : public IRoot
{
	virtual BlueStructureDefinition* GetStructureDefinition() = 0;
	virtual size_t GetMemberCount() = 0;
	virtual size_t GetStructureSize() = 0;
	virtual size_t GetSize() = 0;
	virtual const void* GetDefaultValue() = 0;
	virtual void* GetAt( size_t ix ) = 0;
	virtual bool Append( const void* val ) = 0;
	virtual bool Insert( size_t ix, const void* val ) = 0;
	virtual bool Remove( size_t ix ) = 0;
	virtual void Clear() = 0;
	virtual void Resize( size_t numItems ) = 0;
	virtual void ItemChanged( size_t ix ) = 0;

	virtual IBlueStructureListNotify* SetNotify( IBlueStructureListNotify* notify ) = 0;
};
#endif // IBlueStructureList_h