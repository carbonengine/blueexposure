// Copyright © 2026 CCP ehf.

#pragma once
#ifndef TestCallbacks_H
#define TestCallbacks_H

BLUE_CLASS( TestCallbacks ) : public IRoot
{
public:
	EXPOSE_TO_BLUE();

	TestCallbacks( IRoot* lockobj = nullptr );

	const BlueScriptCallback& GetCallback() const;
	void SetCallback( const BlueScriptCallback& callback );

	bool HasValidCallback() const;

	bool CallCallbackVoid();
	std::pair<bool, int> CallCallbackInt();
	std::pair<bool, std::string> CallCallbackStringInt( int arg0 );
	std::pair<bool, int> CallCallbackIntStringFloat( const std::string& arg0, float arg1 );
private:
	BlueScriptCallback m_callback;
};

TYPEDEF_BLUECLASS( TestCallbacks );

#endif