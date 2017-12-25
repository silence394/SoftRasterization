#include "color.h"
#include "vector4.h"

Color::Color( uint color )
{
	r = ( ( color & 0x00ff0000 ) >> 16 ) / 255.0f;
	g = ( ( color & 0x0000ff00 ) >> 8 ) / 255.0f;
	b = ( color & 0x000000ff ) / 255.0f;
	a = ( ( color & 0xff000000 ) >> 24 ) / 255.0f;
}

Color::operator Vector4( ) const
{
	return Vector4( r, g, b, a );
}