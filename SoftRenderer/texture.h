#pragma once

#include "prerequisites.h"
#include "surface.h"
#include <vector>

class Texture
{
public:
	enum TextureFormat
	{
		TF_ARGB8	= 1,
		TF_RGB8		= 2,
	};

private:
	void*					mBuffer;
	uint					mWidth;
	uint					mHeight;
	PixelFormat				mFormat;
	std::vector<SurfacePtr>	mSurfaces;

public:
	Texture( ) : mBuffer( nullptr ), mWidth( 0 ), mHeight( 0 ), mFormat( 0 ) { }

	Texture( void* buffer, uint width, uint height, PixelFormat format ) : mBuffer( buffer ), mWidth( width ), mHeight( height ), mFormat( format ) { }

	Texture( uint width, uint height, PixelFormat format );

	static uint Texture::GetBpp( PixelFormat format );

public:
	uint GetPixel( uint x, uint y );
	uint GetPixelbyUV( float u, float v );

	uint GetWidth( ) const
	{
		return mWidth;
	}

	uint GetHeight( ) const
	{
		return mHeight;
	}

	SurfacePtr	GetSurface( uint index )
	{
		return mSurfaces[index];
	}

	Color		Sample( )
	{}
};