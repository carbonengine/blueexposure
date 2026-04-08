// Copyright © 2026 CCP ehf.

#include "include/BlueRegistration.h"

// --------------------------------------------------------------------------------------
// Description:
//   Adds classes, module functions and enumerations to the module at initialization
// --------------------------------------------------------------------------------------
bool GetEnumValuesAsBitMask_Impl( const EnumValues &values, uint32_t val, std::string& out )
{
	out.clear();
	out.reserve( 30 ); 

	// Some Enums that are used as bit-masks define convenience values that are
	// combinations of many values. If there's an exact match, always return just that.
	for( EnumValues::const_iterator i = values.begin(); i != values.end(); ++i )
	{
		if( i->mValue.mLong == val )
		{                                              
			out = i->mKey;
			return true;
		}
	}

	bool foundMatch = false;
	// If there's no exact match, work out the names of the values ORed in the given value
	for( EnumValues::const_iterator i = values.begin(); i != values.end(); ++i )
	{
		// The only match with an enum bitmask of value '0' is itself (see above)
		if( i->mValue.mLong && (i->mValue.mLong & val) == i->mValue.mLong )
		{
			if( !out.empty() )
			{
				out.append( " | " );
			}
			out.append( i->mKey );
			foundMatch = true;
		}
	}

	return foundMatch;
}

 std::string GetEnumValueNameAsBitMask( GetEnumValuesFunctionTypePtr f, uint32_t val )
{
	std::string result;

    if( !f )
    {
		return result;
    }
	
    const EnumValues& values = f();
	GetEnumValuesAsBitMask_Impl(values, val, result);
	return result;
}

bool GetEnumValueName_Impl( const EnumValues &values, uint32_t val, std::string& out )
 {
	 out.clear();
	 out.reserve( 30 ); 

	 bool foundMatch = false;
	 for( EnumValues::const_iterator i = values.begin(); i != values.end(); ++i )
	 {
		 if( i->mValue.mLong == val )
		 {
			 if( !out.empty() )
			 {
				 out.append( " | " );
			 }
			 out.append( i->mKey );
			 foundMatch = true;
		 }
	 }

	 return foundMatch;
 }

std::string GetEnumValueName( GetEnumValuesFunctionTypePtr f, uint32_t val )
{
	std::string result;

	if( !f )
	{
		return result;
	}

	const EnumValues& values = f();
	GetEnumValueName_Impl( values, val, result );
	return result;
}

bool ClassRegistrarNullFactory(const Be::IID& riid, void** ppv)
{
	return false;
}

ThunkerRegsVector& BlueRegistration::GetGlobalThunkerRegs()
{
	static ThunkerRegsVector s_globalThunkerRegs;
	return s_globalThunkerRegs;
}
