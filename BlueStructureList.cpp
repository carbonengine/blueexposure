// Copyright © 2026 CCP ehf.

#include "include/BlueStructureList.h"
#include "include/BlueTypeTraits.h"
#include "TypeInfo.h"

namespace
{

#ifdef _MSC_VER

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

inline bool isinf( double num ) 
{ 
	return !_finite( num ) && !_isnan( num );
}

inline bool isnan( double num ) 
{ 
	return !!_isnan( num ); 
}

inline bool signbit( double num ) 
{ 
	return _copysign( 1.0, num ) < 0; 
}

#elif defined( __ANDROID__ )

inline bool isinf( double num )
{
	return std::isinf( num );
}

inline bool signbit( double num )
{
	return std::signbit( num );
}

#elif !defined( __APPLE__)

inline bool signbit( double num ) 
{ 
    return std::signbit( num );
}

#endif
}

#if BLUE_WITH_PYTHON



namespace
{


template<typename T>
T ConvertDataTypeToBuiltinType( const T& value )
{
	return value;
}

const char* ConvertDataTypeToBuiltinType( const BlueSharedString& value )
{
	return value.c_str();
}

template<typename DataType, typename PyConvertionFunction>
PyObject* GetStructureElement( const void* member, int size, PyConvertionFunction* convertionFunction )
{
	PyObject* returnValue = nullptr;
	if( size > 1 )
	{
		returnValue = PyTuple_New( size );
	}

	for( int compIx = 0; compIx < size; ++compIx )
	{
		DataType val = *static_cast<const DataType*>( member );
		member = static_cast<const uint8_t*>( member ) + sizeof( DataType );

		PyObject* compVal = ( *convertionFunction )( ConvertDataTypeToBuiltinType( val ) );

		if( size > 1 )
		{
			PyTuple_SET_ITEM( returnValue, compIx, compVal );
		}
		else
		{
			returnValue = compVal;
		}
	}
	return returnValue;
}

PyObject* PyFloat_FromFloat16( uint16_t float16 )
{
	return PyFloat_FromDouble( BlueFloat16To32( float16 ) );
}

uint16_t PyFloat_AsFloat16( PyObject* float32 )
{
	return BlueFloat32To16( float( PyFloat_AsDouble( float32 ) ) );
}

PyObject* PyTuple_FromVector4( const Vector4& value )
{
	PyObject* result = PyTuple_New( 4 );
	PyTuple_SET_ITEM( result, 0, PyFloat_FromDouble( value.x ) );
	PyTuple_SET_ITEM( result, 1, PyFloat_FromDouble( value.y ) );
	PyTuple_SET_ITEM( result, 2, PyFloat_FromDouble( value.z ) );
	PyTuple_SET_ITEM( result, 3, PyFloat_FromDouble( value.w ) );
	return result;
}

Vector4 PyTuple_AsVector4( PyObject* tuple )
{
	Vector4 result = { 0, 0, 0, 0 };
	if( !PyTuple_Check( tuple ) )
	{
		PyErr_SetString( PyExc_TypeError, "expected a tuple of floats" );
		return result;
	}
	result.x = float( PyFloat_AsDouble( PyTuple_GetItem( tuple, 0 ) ) );
	result.y = float( PyFloat_AsDouble( PyTuple_GetItem( tuple, 1 ) ) );
	result.z = float( PyFloat_AsDouble( PyTuple_GetItem( tuple, 2 ) ) );
	result.w = float( PyFloat_AsDouble( PyTuple_GetItem( tuple, 3 ) ) );
	return result;
}

bool PyBool_AsBool( PyObject* boolean )
{
	if( boolean != Py_True && boolean != Py_False )
	{
		PyErr_SetString( PyExc_TypeError, "expected a boolean" );
		return false;
	}
	return boolean == Py_True;
}

PyObject* GetStructureElement( const uint8_t* item, const BlueStructureDefinition* memberDef )
{
	int type = memberDef->m_dataType & Be::DT_TYPE_MASK;
	int size = ((memberDef->m_dataType & Be::DT_SIZE_MASK) >> Be::DT_SIZE_OFFSET) + 1;
	const uint8_t* member = item + memberDef->m_offset;

	PyObject* returnValue = nullptr;

	// FLOAT32_3 and similar types are returned as tuples
	switch( type )
	{
	case Be::DT_INT8:
		if( memberDef->m_dataType & Be::DT_UNSIGNED_BIT )
		{
			returnValue = GetStructureElement<uint8_t, decltype( PyLong_FromSize_t )>( member, size, &PyLong_FromSize_t );
		}
		else
		{
			returnValue = GetStructureElement<int8_t, decltype( PyLong_FromLong )>( member, size, &PyLong_FromLong );
		}
		break;

	case Be::DT_INT16:
		if( memberDef->m_dataType & Be::DT_UNSIGNED_BIT )
		{
			returnValue = GetStructureElement<uint16_t, decltype( PyLong_FromSize_t )>( member, size, &PyLong_FromSize_t );
		}
		else
		{
			returnValue = GetStructureElement<int16_t, decltype( PyLong_FromLong )>( member, size, &PyLong_FromLong );
		}
		break;

	case Be::DT_INT32:
		if( memberDef->m_dataType & Be::DT_UNSIGNED_BIT )
		{
			returnValue = GetStructureElement<uint32_t, decltype( PyLong_FromSize_t )>( member, size, &PyLong_FromSize_t );
		}
		else
		{
			returnValue = GetStructureElement<int32_t, decltype( PyLong_FromLong )>( member, size, &PyLong_FromLong );
		}
		break;

	case Be::DT_FLOAT16:
		returnValue = GetStructureElement<uint16_t, decltype( PyFloat_FromFloat16 )>( member, size, &PyFloat_FromFloat16 );
		break;

	case Be::DT_FLOAT32:
		returnValue = GetStructureElement<float, decltype( PyFloat_FromDouble )>( member, size, &PyFloat_FromDouble );
		break;

	case Be::DT_FLOAT32x4:
		returnValue = GetStructureElement<Vector4, decltype( PyTuple_FromVector4 )>( member, size, &PyTuple_FromVector4 );
		break;

	case Be::DT_BOOL8:
		returnValue = GetStructureElement<bool, decltype( PyBool_FromLong )>( member, size, &PyBool_FromLong );
		break;

	case Be::DT_SHAREDSTRING:
		returnValue = GetStructureElement<BlueSharedString, decltype( PyString_FromString )>( member, size, &PyString_FromString );
		break;
	}
	return returnValue;
}

// This craziness is to avoid "comparison of unsigned value to 0" warnins on clang
template <typename Dest, typename Src, bool IsSigned>
struct CheckLowerBoundHelper
{
    bool operator()( Src src )
    {
        return src >= std::numeric_limits<Dest>::min();
    }
};

template <typename Dest, typename Src>
struct CheckLowerBoundHelper<Dest, Src, false>
{
    bool operator()( Src src )
    {
        return true;
    }
};

template <typename Dest, typename Src>
bool CheckLowerBound( Src src )
{
    return CheckLowerBoundHelper<Dest, Src, std::numeric_limits<Src>::is_signed>()( src );
}
    
template <typename Dest, typename Src>
void SafeCastToType( Dest& dest, Src src )
{
	if( !CheckLowerBound<Dest, Src>( src ) || src > std::numeric_limits<Dest>::max() )
	{
		throw std::runtime_error( "value out of range for structure element" );
	}
	dest = static_cast<Dest>( src );
}

template <typename Src>
void SafeCastToType( float& dest, Src src )
{
	if( src < -std::numeric_limits<float>::max() || src > std::numeric_limits<float>::max() )
	{
		throw std::runtime_error( "value out of range for structure element" );
	}
	dest = static_cast<float>( src );
}

void SafeCastToType( BlueSharedString& dest, char* src )
{
	dest = BlueSharedString( src );
}

void SafeCastToType( Vector4& dest, const Vector4& src )
{
	dest = src;
}

template<typename DataType, typename PyConvertionFunction>
void ExtractItem( PyObject* memberObject, void* member, int size, PyConvertionFunction* conversionFunction )
{
	if( size > 1 )
	{
		for( int compIx = 0; compIx < size; ++compIx )
		{
			PyObject* compObject = PyTuple_GetItem( memberObject, compIx );
			auto src = ( *conversionFunction )( compObject );
			if( PyErr_Occurred() )
			{
				throw std::runtime_error( "value can not be converted to structure" );
			}
			DataType compVal;
			SafeCastToType( compVal, src );
			*static_cast<DataType*>( member ) = compVal;
			member = static_cast<uint8_t*>( member ) + sizeof( DataType );
		}
	}
	else
	{
		auto src = ( *conversionFunction )( memberObject );
		if( PyErr_Occurred() )
		{
			throw std::runtime_error( "value can not be converted to structure" );
		}
		DataType compVal;
		SafeCastToType( compVal, src );
		*static_cast<DataType*>( member ) = compVal;
	}
}

void ConvertStructureDefinitionToBeVar( const BlueStructureDefinition& record, Be::VarEntry& var )
{
	var.mName = record.m_name;
	var.mOffset = record.m_offset;
	var.mDescription = "";
	var.mEditFlags = 0;
	var.mChooserTable = record.m_chooser;
	var.mGetProperty = nullptr;
	var.mSetProperty = nullptr;
	var.mIID = nullptr;
	switch( record.m_dataType & Be::DT_TYPE_MASK )
	{
	case Be::DT_INT8:
		var.mType = Be::BYTE;
		var.mSize = 1;
		break;
	case Be::DT_INT16:
		var.mType = Be::SHORT;
		var.mSize = 2;
		break;
	case Be::DT_INT32:
		var.mType = ( record.m_dataType & Be::DT_SIZE_MASK ) ? Be::INTARRAY : Be::LONG;
		var.mSize = ( ( ( record.m_dataType & Be::DT_SIZE_MASK ) >> Be::DT_SIZE_OFFSET ) + 1 ) * 4;
		break;
	case Be::DT_FLOAT32:
		var.mType = Be::FLOAT;
		var.mSize = ( ( ( record.m_dataType & Be::DT_SIZE_MASK ) >> Be::DT_SIZE_OFFSET ) + 1 ) * 4;
		break;
	case Be::DT_SHAREDSTRING:
		var.mType = Be::SHAREDSTRING;
		break;
	case Be::DT_FLOAT32x4:
		var.mType = Be::FLOATARRAY;
		var.mSize = 16 * sizeof( float );
		var.mIID = &BlueMatrixIID;
		break;
	case Be::BOOL8_1:
		var.mType = Be::BOOL;
		break;
	}
}

}

BLUEIMPORT PyObject* BlueStructureList_StructurePyObject( IBlueStructureList* structureList, uint8_t* item )
{
	//
	// Build the return value. It is usually a tuple, except in the degenerate case of a structure
	// with only one member. Each member is again potentially a tuple - a Vector3, described as
	// a FLOAT32_3 will be returned as a 3-tuple, for example.
	//
	BlueStructureDefinition* sd = structureList->GetStructureDefinition();
	BlueStructureDefinition* memberDef = sd;

	size_t memberCount = structureList->GetMemberCount();

	PyObject* returnValue = nullptr;
	if( memberCount > 1 )
	{
		returnValue = PyTuple_New( memberCount );
	}

	int ix = 0;
	while( memberDef->m_name )
	{
		PyObject* memberTuple = GetStructureElement( item, memberDef );
		if( !memberTuple )
		{
			Py_XDECREF( returnValue );
			PyErr_SetString( PyExc_RuntimeError, "unable to convert structure list to python" );
			return nullptr;
		}
		if( returnValue )
		{
			PyTuple_SET_ITEM( returnValue, ix, memberTuple );
		}
		else
		{
			returnValue = memberTuple;
		}

		++ix;
		++memberDef;
	}

	return returnValue;
}

BLUEIMPORT void BlueStructureList_PyObjectToStructure( IBlueStructureList* structureList, PyObject* value, uint8_t* item )
{
	size_t memberCount = structureList->GetMemberCount();

	if( memberCount > 1 )
	{
		if( !PyTuple_Check( value ) )
		{
			throw std::runtime_error( "value can not be converted to structure" );
		}
		if( PyTuple_Size( value ) != memberCount )
		{
			throw std::runtime_error( "value can not be converted to structure" );
		}
	}

	int ix = 0;
	BlueStructureDefinition* memberDef = structureList->GetStructureDefinition();

	while( memberDef->m_name )
	{
		PyObject* memberObject;
		if( memberCount > 1 )
		{
			memberObject = PyTuple_GetItem( value, ix );
		}
		else
		{
			memberObject = value;
		}

		int type = memberDef->m_dataType & Be::DT_TYPE_MASK;
		int size = ((memberDef->m_dataType & Be::DT_SIZE_MASK) >> Be::DT_SIZE_OFFSET) + 1;
		uint8_t* member = item + memberDef->m_offset;

		if( size > 1 )
		{
			if( !PyTuple_Check( memberObject ) )
			{
				throw std::runtime_error( "value can not be converted to structure" );
			}
			if( PyTuple_Size( memberObject ) != size )
			{
				throw std::runtime_error( "value can not be converted to structure" );
			}
		}

		switch( type )
		{
		case Be::DT_INT8:
			if( memberDef->m_dataType & Be::DT_UNSIGNED_BIT )
			{
				ExtractItem<uint8_t, decltype( PyLong_AsUnsignedLong )>( memberObject, member, size, &PyLong_AsUnsignedLong );
			}
			else
			{
				ExtractItem<int8_t, decltype( PyLong_AsLong )>( memberObject, member, size, &PyLong_AsLong );
			}
			break;
		case Be::DT_INT16:
			if( memberDef->m_dataType & Be::DT_UNSIGNED_BIT )
			{
				ExtractItem<uint16_t, decltype( PyLong_AsUnsignedLong )>( memberObject, member, size, &PyLong_AsUnsignedLong );
			}
			else
			{
				ExtractItem<int16_t, decltype( PyLong_AsLong )>( memberObject, member, size, &PyLong_AsLong );
			}
			break;
		case Be::DT_INT32:
			if( memberDef->m_dataType & Be::DT_UNSIGNED_BIT )
			{
				ExtractItem<uint32_t, decltype( PyLong_AsUnsignedLong )>( memberObject, member, size, &PyLong_AsUnsignedLong );
			}
			else
			{
				ExtractItem<int32_t, decltype( PyLong_AsLong )>( memberObject, member, size, &PyLong_AsLong );
			}
			break;
		case Be::DT_FLOAT16:
			ExtractItem<uint16_t, decltype( PyFloat_AsFloat16 )>( memberObject, member, size, &PyFloat_AsFloat16 );
			break;
		case Be::DT_FLOAT32:
			ExtractItem<float, decltype( PyFloat_AsDouble )>( memberObject, member, size, &PyFloat_AsDouble );
			break;
		case Be::DT_SHAREDSTRING:
			ExtractItem<BlueSharedString, decltype( PyString_AsString )>( memberObject, member, size, &PyString_AsString );
			break;
		case Be::DT_FLOAT32x4:
			ExtractItem<Vector4, decltype( PyTuple_AsVector4 )>( memberObject, member, size, &PyTuple_AsVector4 );
			break;
		case Be::DT_BOOL8:
			ExtractItem<bool, decltype( PyBool_AsBool )>( memberObject, member, size, &PyBool_AsBool );
			break;
		default:
			{
				throw std::runtime_error( "type is not supported" );
			}
			break;
		}
		++ix;
		++memberDef;
	}
}

BLUEIMPORT PyObject* BlueStructureList_PyGetStructureDefinition( IBlueStructureList* structureList )
{
	size_t memberCount = structureList->GetMemberCount();
	PyObject* ret = PyTuple_New( memberCount );
	BlueStructureDefinition* memberDef = structureList->GetStructureDefinition();
	for( size_t i = 0; i < memberCount; i++ )
	{
		PyObject* item = PyTuple_New( 4 );
		PyTuple_SET_ITEM( item, 0, PyString_FromString( memberDef[i].m_name ) );
		PyTuple_SET_ITEM( item, 1, PyLong_FromLong( memberDef[i].m_dataType ) );
		PyTuple_SET_ITEM( item, 2, PyLong_FromLong( memberDef[i].m_offset ) );
		if( memberDef[i].m_chooser )
		{
			Be::VarEntry var;
			ConvertStructureDefinitionToBeVar( memberDef[i], var );
			PyTuple_SET_ITEM( item, 3, PyGetChooserInfo( var ) );
		}
		else
		{
			PyTuple_SET_ITEM( item, 3, Py_None );
			Py_INCREF( Py_None );
		}
		PyTuple_SET_ITEM( ret, i, item );	
	}
	return ret;
}

#endif

float BlueFloat16To32( uint16_t in )
{
	const uint16_t s = ( in & 0x8000 );
    const uint16_t e = ( in & 0x7C00 ) >> 10;
    const uint16_t m = in & 0x3FF;
    const float sgn = (s ? -1.0f : 1.0f);

    if( ( in & ~0x8000 ) == 0x7fff )
	{
		return s ? -std::numeric_limits<float>::quiet_NaN() : std::numeric_limits<float>::quiet_NaN();
	}

    if( e == 0 )
    {
        if( m == 0 ) 
		{
			return sgn * 0.0f; /* +0.0 or -0.0 */
		}
        else 
		{
			return sgn * powf( 2, -14.0f ) * ( m / 1024.0f );
		}
    }
    else
    {
        return sgn * powf( 2, e - 15.0f ) * ( 1.0f + ( m / 1024.0f ) );
    }
}

uint16_t BlueFloat32To16( float in )
{
    int exp = 0, origexp;
    float tmp = fabs( in );
    int sign = signbit( in );
    uint32_t mantissa;
    uint16_t ret;

    /* Deal with special numbers */
    if( isinf( in ) )
	{
		return ( sign ? 0xffff : 0x7fff );
	}
    if( isnan( in ) ) 
	{
		return ( sign ? 0xffff : 0x7fff );
	}
    if( in == 0.0f ) 
	{
		return ( sign ? 0x8000 : 0x0000 );
	}

    if( tmp < powf( 2, 10 ) )
    {
        do
        {
            tmp *= 2.0f;
            exp--;
        } while( tmp < powf( 2, 10 ) );
    }
    else if( tmp >= powf( 2, 11 ) )
    {
        do
        {
            tmp /= 2.0f;
            exp++;
        } while( tmp >= powf( 2, 11 ) );
    }

    exp += 10;  /* Normalize the mantissa */
    exp += 15;  /* Exponent is encoded with excess 15 */

    origexp = exp;

    mantissa = (uint32_t) tmp;
    if( ( tmp - mantissa == 0.5f && mantissa % 2 == 1 ) || /* round half to even */
        ( tmp - mantissa > 0.5f ) )
    {
        mantissa++; /* round to nearest, away from zero */
    }
    if( mantissa == 2048 )
    {
        mantissa = 1024;
        exp++;
    }

    if( exp > 31 )
    {
        /* too big */
        ret = 0x7fff; /* INF */
    }
    else if( exp <= 0 )
    {
        uint32_t rounding = 0;

        /* Denormalized half float */

        /* return 0x0000 (=0.0) for numbers too small to represent in half floats */
        if( exp < -11 )
		{
            return (sign ? 0x8000 : 0x0000);
		}
        exp = origexp;

        /* the 13 extra bits from single precision are used for rounding */
        mantissa = (uint32_t)( tmp * powf( 2, 13 ) );
        mantissa >>= 1 - exp; /* denormalize */

        mantissa -= ~( mantissa >> 13 ) & 1; /* round half to even */
        /* remove 13 least significant bits to get half float precision */
        mantissa >>= 12;
        rounding = mantissa & 1;
        mantissa >>= 1;

        ret = mantissa + rounding;
    }
    else
    {
        ret = ( exp << 10 ) | ( mantissa & 0x3ff );
    }

    ret |= ( ( sign ? 1 : 0 ) << 15 ); /* Add the sign */
    return ret;
}
