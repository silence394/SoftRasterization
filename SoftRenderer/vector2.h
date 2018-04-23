#pragma once

class Vector2
{
public:
	float x, y;

public:
	Vector2( )
		: x( 0.0f ), y( 0.0f ) { }

	Vector2( float xx, float yy )
		: x( xx ), y( yy ) { }

	Vector2& operator *= ( float w )
		{ x *= w; y *= w; return *this; }
	Vector2& operator /= ( float w )
		{ float invw = 1.0f / w; x *= invw; y *= invw; return *this; }
};