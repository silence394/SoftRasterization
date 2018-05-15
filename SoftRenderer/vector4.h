#pragma once

#include "Prerequisites.h"
#include "Vector3.h"

class Vector4
{
public:
	float x, y, z, w;

public:
	Vector4( ) { }

	Vector4( float f ) : x( f ), y( f ), z( f ), w( f ) { }

	Vector4( float xx, float yy, float zz, float ww ) : x( xx ), y( yy ), z( zz ), w( ww ) { }

	Vector4( const Vector3& vec3, float ww ) : x( vec3.x ), y( vec3.y ), z( vec3.z ), w( ww ) { }

	static Vector4 Lerp( const Vector4& v1, const Vector4& v2, float factor )
	{
		return v1 + ( v2 - v1 ) * factor;
	}

	Vector4 operator + ( const Vector4& v ) const
	{
		return Vector4( x + v.x, y + v.y, z + v.z, w + v.w );
	}

	Vector4 operator - ( const Vector4& v ) const
	{
		return Vector4( x - v.x, y - v.y, z - v.z, w - v.w );
	}

	Vector4 operator * ( float f ) const
	{
		return Vector4( x * f, y *f, z * f, w *f );
	}

	Vector4& operator *= ( float f )
	{
		x *= f; y *= f; z *= f; w *= f;
		return *this;
	}

	Vector4 operator / ( float div ) const
	{
		float inv = 1.0f / div;
		return Vector4( x * inv, y * inv, z * inv, w * inv );
	}

	Vector4 operator /= ( float div )
	{
		float inv = 1.0f / div;  x *= inv; y *= inv; z *= inv; w *= inv;
		return *this;
	}

	float Dot( const Vector4& v ) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	float Magnitude( ) const
	{
		return Math::Sqrt( x * x + y * y + z * z + w * w );
	}

	Vector4& Normalize( )
	{
		float m = Magnitude( );
		if ( m > 0.0f )
		{
			x /= m; y /= m; z /= m; w /= m;
		}
		
		return *this;
	}

	Vector4& operator *= ( const Matrix4& mat );

	operator Color( ) const;
};