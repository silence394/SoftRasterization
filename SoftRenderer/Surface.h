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

	inline uint Pitch( ) const
	{
		return mWidth * mBPP;
	}

	inline uint Offset( int x, int y ) const
	{
		return y * Pitch( );
	}

public:
	void*	Address( uint x, uint y )
	{
		return reinterpret_cast<void*>( mBuffer.data( ) + Offset( x, y ) );
	}

	Color	Sample( uint x, uint y )
	{
		return mConvertor( Address( x, y ) );
	}
};