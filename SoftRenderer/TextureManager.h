#pragma once

#include "prerequisites.h"

class TextureManager
{
public:
	static TexturePtr Load( const std::wstring& resname );
};