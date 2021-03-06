#pragma once

#include "Prerequisites.h"
#include "Math.h"

class Color
{
public:
	float r, g, b, a;

public:
	Color( ) : r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 0.0f ) { }

	Color( float red, float green, float blue, float alpha ) : r( red ), g( green ), b( blue ), a( alpha ) { }

	Color( uint color );

	static Color Lerp( const Color& c1, const Color& c2, float f )
	{
		return c1 + ( c2 - c1 ) * f;
	}

	Color operator + ( const Color& c ) const
	{
		return Color( r + c.r, g + c.g, b + c.b, a + c.a );
	}

	Color operator - ( const Color& c ) const
	{
		return Color( r - c.r, g - c.g, b - c.b, a - c.a );
	}

	Color operator - ( float f ) const
	{
		return Color( r - f, g - f, b - f, a - f );
	}

	Color operator * ( float f ) const
	{
		return Color ( r * f, g * f, b * f, a * f );
	}

	Color& operator *= ( float f )
	{
		r *= f; g *= f; b *= f; a *= f; return *this;
	}

	Color operator * ( const Color& c ) const
	{
		return Color( r * c.r, g * c.g, b * c.b, a * c.a );
	}

	Color& operator *= ( const Color& c )
	{
		r *= c.r; g *= c.g; b *= c.b; a *= c.a;
		return *this;
	}

	inline operator uint( ) const
	{
		byte rr = (byte) ( Math::Clamp( r, 0.0f, 1.0f ) * 255.0f );
		byte gg = (byte) ( Math::Clamp( g, 0.0f, 1.0f ) * 255.0f );
		byte bb = (byte) ( Math::Clamp( b, 0.0f, 1.0f ) * 255.0f );
		byte aa = (byte) ( Math::Clamp( a, 0.0f, 1.0f ) * 255.0f );
		return ( aa << 24 ) | ( rr << 16 ) | ( gg << 8 ) | bb;
	}

	Color& ToColor( )
	{
		return *this;
	}

	template <typename T>
	Color& operator = ( const T& c )
	{
		*this = c.ToColor( );
		return *this;
	}

	operator Vector4( ) const;
};

class RGBA8
{
public:
	byte r;
	byte g;
	byte b;
	byte a;

	RGBA8( byte rr, byte gg, byte bb, byte aa ) : r( rr ), g( gg ), b( bb ), a( aa ) { }

	Color ToColor( ) const
	{
		const float inv255 = 1.0f / 255;
		return Color( r * inv255, g * inv255, b * inv255, a * inv255 );
	}
};

class RGB8
{
public:
	byte r;
	byte g;
	byte b;

	RGB8( byte rr, byte gg, byte bb, byte aa ) : r( rr ), g( gg ), b( bb ) { }

	Color ToColor( ) const
	{
		const float inv255 = 1.0f / 255;
		return Color( r * inv255, g * inv255, b * inv255, 1.0f );
	}
};