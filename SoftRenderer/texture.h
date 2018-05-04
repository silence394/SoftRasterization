#pragma once

#include "prerequisites.h"

class Texture
{
public:
	enum TextureFormat
	{
		TF_ARGB8	= 1,
		TF_RGB8		= 2,
	};

private:
	void*	mBuffer;
	uint	mWidth;
	uint	mHeight;
	uint	mFormat;

public:
	Texture( )
		: mBuffer( nullptr ), mWidth( 0 ), mHeight( 0 ), mFormat( 0 ) { }

	Texture( void* buffer, uint width, uint height, uint format )
		: mBuffer( buffer ), mWidth( width ), mHeight( height ), mFormat( format ) { }

	~Texture( )
		{ delete[] mBuffer; }

	static uint Texture::GetBpp( uint format );

	uint GetPixel( uint x, uint y );
	uint GetPixelbyUV( float u, float v );

	uint GetWidth( ) const
		{ return mWidth; }
	uint GetHeight( ) const
		{ return mHeight; }
};