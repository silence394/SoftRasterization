#pragma once

#include <FreeImage.h>

template< typename ColorType >
class FreeImageColor
{
};

template<>
class FreeImageColor<RGB8>
{
public:
	byte r, g, b, a;

	FreeImageColor( const byte* c )
		: r( c[ FI_RGBA_RED ] ), g( c [ FI_RGBA_GREEN ] ), b( c[ FI_RGBA_BLUE ] ), a( 0xff ) { }
};

template<>
class FreeImageColor<RGBA8>
{
public:
	byte r, g, b, a;

	FreeImageColor( const byte* c )
		: r( c[ FI_RGBA_RED ] ), g( c [ FI_RGBA_GREEN ] ), b( c[ FI_RGBA_BLUE ] ), a( c[ FI_RGBA_ALPHA ] ) { }
};