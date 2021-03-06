#pragma once

#include <Math.h>

class Math
{
public:
	const static float cEpsilon;
	const static float cPI;
	const static float cHalfPI;
	const static float cQuadPI;

	template< typename T >
	static void Swap( T& t1, T& t2 )
	{
		T temp;
		temp = t1;
		t1 = t2;
		t2 = temp;
	}

	template< typename T >
	static T Clamp( T value, T min, T max )
	{
		if ( value < min ) value = min; if ( value > max ) value = max; return value;
	}

	static float Sqrt( float f )
	{
		return sqrtf( f );
	}

	static float Sin( float r )
	{
		return sinf( r );
	}

	static float Cos( float r )
	{
		return cosf( r );
	}

	static float Floor( float value )
	{
		return ::floorf( value );
	}

	static float Ceil( float value )
	{
		return ::ceilf( value );
	}

	static double Modf( double num, int& intpart )
	{
		double i;
		double frac = ::modf( num, &i );
		intpart = (int) i;

		return frac;
	}

	static float FMod( float x, float y )
	{
		return ::fmodf( x, y );
	}

	template< typename T >
	static T Abs( T val )
	{
		return val >= 0 ? val : -val;
	}

	static float Pow( float base, float exponent )
	{
		if ( base < cEpsilon )
			return 0.0f;

		return ::powf( base, exponent );
	}

	template< typename T >
	static T Max( T t1, T t2 )
	{
		return t1 >= t2 ? t1 : t2;
	}
};