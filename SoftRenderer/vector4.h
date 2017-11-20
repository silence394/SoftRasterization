#pragma once

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

	inline Vector4 operator / ( float div ) const
		{ float inv = 1.0f / div; return Vector4( x * inv, y * inv, z * inv, w * inv ); }
};