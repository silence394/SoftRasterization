#pragma once

#include "Prerequisites.h"

class TextureManager
{
private:
	TextureManager( ) { }

	static std::unique_ptr<TextureManager>	mInstance;

public:
	static TextureManager&	Instance( );
	TexturePtr				Load( const std::wstring& resname );
};