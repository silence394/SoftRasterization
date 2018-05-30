#include "Color.h"
#include "Vector4.h"

Color::Color( uint color )
{
	const float inv = 1.0f / 255.0f;
	r = ( ( color & 0x00ff0000 ) >> 16 ) * inv;
	g = ( ( color & 0x0000ff00 ) >> 8 ) * inv;
	b = ( color & 0x000000ff ) * inv;
	a = ( ( color & 0xff000000 ) >> 24 ) * inv;
}

Color::operator Vector4( ) const
{
	return Vector4( r, g, b, a );
}