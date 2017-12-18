#include "renderer.h"

Texture::Texture( void* buffer, uint width, uint height, uint format )
	: mWidth( width ), mHeight( height ), mFormat( format )
{
	uint bpp = GetBpp( format );
	if ( bpp == 0 )
		return;

	uint length = width * height * bpp;
	mBuffer = new char[ length ];
	memcpy( mBuffer, buffer, length );
}

Texture::~Texture( )
{
	delete[] mBuffer;
}

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