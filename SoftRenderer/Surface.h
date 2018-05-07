#pragma once

#include "prerequisites.h"

class Surface
{
private:
	uint	mWidth;
	uint	mHeight;
	uint	mFormat;
	uint	mBPP;

	std::vector<byte>	mBuffer;

public:
	Surface( uint w, uint h, uint format );
};