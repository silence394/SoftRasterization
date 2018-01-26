#include "texture.h"
#include <string>
#include "math.h"

uint Texture::GetBpp( uint format )
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
	x = Math::Clamp( x, 0.0f, 1.0f ) * (float) mWidth;
	y = Math::Clamp( y, 0.0f, 1.0f ) * (float) mHeight;

	return *( (uint*) mBuffer + mWidth * (uint) y + (uint) x );
}