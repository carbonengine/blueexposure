// Copyright © 2026 CCP ehf.

#pragma once
#ifndef TestAttributes_h
#define TestAttributes_h

#include "BlueExposure.h"

BLUE_DECLARE( TestAttributes );
BLUE_DECLARE_VECTOR( TestAttributes );
BLUE_DECLARE_DICT( TestAttributes );

BLUE_CLASS( TestAttributes ) : public IRoot
{
public:
	EXPOSE_TO_BLUE();

	TestAttributes( IRoot* lockobj = nullptr );

private:
	std::string m_myString;
	std::wstring m_myUnicode;
	bool m_myBool;
	int m_myInt;
	float m_myFloat;
	double m_myDouble;
	int64_t m_myInt64;
	BlueSharedString m_sharedString;
	BlueSharedStringW m_sharedStringW;

	PTestAttributesVector m_myVector;
#if BLUE_WITH_PYTHON
	PTestAttributesDict m_myDict;
#endif

};

TYPEDEF_BLUECLASS( TestAttributes );

#endif // TestAttributes_h