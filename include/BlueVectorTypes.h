// Copyright © 2026 CCP ehf.

/* 
	*************************************************************************************

	BlueVectorTypes.h

	Creator:	Halldor Fannar
	Created:	2008-10-30
	Project:	Blue

	Note: This is a sneaky file - it is used to allow Blue to handle vectors and matrices
	gracefully wihtout really knowing anything about your vector and matrix classes!  

	(c) CCP 2008

	*************************************************************************************
*/

#pragma once
#ifndef BLUEVECTORTYPES_H
#define BLUEVECTORTYPES_H

//------------------------------------------------------------------------
// If BLUE_OVERRIDE_VECTOR_TYPES macro has been defined, the vector and
// matrix classes are not defined here - the user must provide his own 
// implementation of the classes below. The data layout of the vectors and
// matrices must be bit-exact with the following definitions.
// NOTE: If you want to override these you must add
//
//  BLUE_OVERRIDE_VECTOR_TYPES=1
//
// to your project defines.  You must also be careful to include your
// own definitions for these types before including any Blue files.
//------------------------------------------------------------------------

#if !BLUE_OVERRIDE_VECTOR_TYPES 

struct Vector2    { float x, y; };
struct Vector2d   { double x, y; };
struct Vector3    { float x, y, z; };
struct Vector3d   { double x, y, z; };
struct Vector3i   { int x, y, z; };
struct Vector4    { float x, y, z, w; };
struct Vector4d   { double x, y, z, w; };
struct Quaternion { float x, y, z, w; };
struct Color 	  { float r, g, b, a; };
struct Matrix	 
{ 
	union{ 
		float m[4][4]; 
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
	};
};

#endif // not BLUE_OVERRIDE_VECTOR_TYPES


#endif