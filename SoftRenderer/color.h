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

	Color operator * ( float f ) const
		{ Color c = *this; c.r *= f; c.g *= f; c.b *= f; c.a *= f; return c; }

	Color& operator*= ( float f )
		{ r *= f; g *= f; b *= f; a *= f; return *this; }

	operator uint( ) const
	{
		unsigned char rr = (unsigned char) ( Math::Clamp( r, 0.0f, 1.0f ) * 255.0f );
		unsigned char gg = (unsigned char) ( Math::Clamp( g, 0.0f, 1.0f ) * 255.0f );
		unsigned char bb = (unsigned char) ( Math::Clamp( b, 0.0f, 1.0f ) * 255.0f );
		unsigned char aa = (unsigned char) ( Math::Clamp( a, 0.0f, 1.0f ) * 255.0f );
		return ( aa << 24 ) | ( rr << 16 ) | ( gg << 8 ) | bb;
	}
};