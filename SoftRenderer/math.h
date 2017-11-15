#pragma once

#include <math.h>

namespace Math
{

	template<typename T>
	static void Swap( T& t1, T& t2 )
	{
		T temp;
		temp = t1;
		t1 = t2;
		t2 = temp;
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

}