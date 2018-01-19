#include "texture.h"
#include <string>

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