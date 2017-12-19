#pragma once

#include "prerequisites.h"
#include "vector3.h"

class Vector4
{
public:
	float x, y, z, w;

public:
	Vector4( )
		{ }

	Vector4( float xx, float yy, float zz, float ww )
		: x( xx ), y( yy ), z( zz ), w( ww ) { }

	Vector4( const Vector3& vec3, float ww )
		: x( vec3.x ), y( vec3.y ), z( vec3.z ), w( ww ) { }

	static Vector4 Lerp( const Vector4& v1, const Vector4& v2, float factor )
		{ return v1 + ( v2 - v1 ) * factor; }

	inline Vector4 operator + ( const Vector4& v ) const
		{ return Vector4( x + v.x, y + v.y, z + v.z, w + v.w ); }

	inline Vector4 operator - ( const Vector4& v ) const
		{ return Vector4( x - v.x, y - v.y, z - v.z, w - v.w ); }

	inline Vector4 operator * ( float f ) const
		{ return Vector4( x * f, y *f, z * f, w *f ); }

	inline Vector4& operator *= ( float f )
		{ x *= f; y *= f; z *= f; w *= f; return *this; }

	inline Vector4 operator / ( float div ) const
		{ float inv = 1.0f / div; return Vector4( x * inv, y * inv, z * inv, w * inv ); }

	float Dot( const Vector4& v ) const
		{ return x * v.x + y * v.y + z * v.z + w * v.w; }
};