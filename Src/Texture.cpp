#include "Texture.h"
#include "Math.h"
#include "Surface.h"

Texture::Texture( uint width, uint height, PixelFormat format ) : mWidth( width ), mHeight( height ), mFormat( format )
{
	mSurfaces.push_back( SurfacePtr( new Surface( width, height, format ) ) );
}

uint Texture::GetBpp( PixelFormat format )
{
	switch ( format )
	{
		case TF_ARGB8:
			return 4;

		default:
			return 0;
	}
}

uint Texture::GetPixel( uint x, uint y )
{
	if ( x >= mWidth || y >= mHeight )
		return 0;

	return *( (uint*) mBuffer + mWidth * y + x );
}

uint Texture::GetPixelbyUV( float x, float y )
{
	// TODO.Clamp is default.
	x *= (float) mWidth;
	y *= (float) mHeight;

	return *( (uint*) mBuffer + mWidth * Math::Clamp( (uint) y, (uint) 0, mHeight - 1 ) + Math::Clamp( (uint) x, (uint) 0, mWidth - 1 ) );
}