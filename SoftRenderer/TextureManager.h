#pragma once

#include "Prerequisites.h"

class TextureManager
{
public:
	static TexturePtr Load( const std::wstring& resname );
};