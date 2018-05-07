#pragma once

#include "prerequisites.h"
#include "PixelFormats.h"

class Surface
{
private:
	uint	mWidth;
	uint	mHeight;
	uint	mFormat;
	uint	mBPP;

	std::vector<byte>	mBuffer;
	PixelFormatConvertor::PixelConvertor	mConvertor;

public:
	Surface( uint w, uint h, uint format );
};