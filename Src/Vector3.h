#pragma once

#include "Prerequisites.h"
#include "Math.h"

class Vector3
{
public:
	float x, y, z;

public:
	Vector3( ) : x( 0.0f ), y( 0.0f ), z( 0.0f ) { }
	Vector3( float xx, float yy, float zz ) : x( xx ), y( yy ), z( zz ) { }

	inline Vector3 operator + ( ) const
	{
		return Vector3( x, y, z );
	}

	inline Vector3 operator - ( ) const
	{
		return Vector3( -x, -y, -z );
	}

	Vector3 operator + ( float var )
	{
		return Vector3( x + var, y + var, z + var );
	}

	Vector3 operator - ( float var )
	{
		return Vector3( x - var, y - var, z - var );
	}

	Vector3 operator * ( float var )
	{
		return Vector3( x * var, y * var, z * var );
	}

	Vector3 operator / ( float var )
	{
		return Vector3( x / var, y / var, z / var );
	}

	Vector3 operator + ( const Vector3& v ) const
	{
		return Vector3( x + v.x, y + v.y, z + v.z );
	}

	Vector3 operator - ( const Vector3& v ) const
	{
		return Vector3( x - v.x, y - v.y, z - v.z );
	}

	Vector3 operator * ( const Vector3& v ) const
	{
		return Vector3( x * v.x, y * v.y, z * v.z );
	}

	Vector3 operator / ( const Vector3& v ) const
	{
		return Vector3( x / v.x, y / v.y, z / v.z );
	}

	Vector3& operator += ( const Vector3& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& operator -= ( const Vector3& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return * this;
	}

	Vector3& operator *= ( const Vector3& v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector3& operator /= ( const Vector3& v )
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	bool operator == ( const Vector3& v )
	{
		return x == v.x && y == v.y && z == v.z;
	}

	static Vector3 Cross( const Vector3& v1, const Vector3& v2 )
	{
		return Vector3( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x );
	}

	static float Dot( const Vector3& v1, const Vector3& v2 )
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	Vector3& operator *= ( const Matrix4& mat );

public:
	float Magnitude( ) const
	{
		return Math::Sqrt( x * x + y * y + z * z );
	}

	Vector3& Normalize( )
	{
		float m = Magnitude( );
		if ( m > 0.0f )
		{
			x /= m;
			y /= m;
			z /= m;
		}

		return *this;
	}
};