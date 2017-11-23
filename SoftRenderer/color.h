#pragma once

class Color
{
public:
	float r, g, b, a;

public:
	Color( )
		: r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 0.0f ) { }

	Color( float red, float green, float blue, float alpha )
		: r( red ), g( green ), b( blue ), a( alpha ) { }

	Color( uint color );

	Color& operator*= ( float f )
		{ r *= f; g *= f; b *= f; a *= f; return *this; }
};