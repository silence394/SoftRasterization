#include "vector3.h"
#include "matrix4.h"

Vector3& Vector3::operator *= ( const Matrix4& mat )
{
	float xx = x, yy = y, zz = z;
	float w = xx * mat[3] + yy * mat[7] + zz * mat[11] + 1.0f * mat[15];

	x = xx * mat[0] + yy * mat[4] + zz * mat[8] + 1.0f * mat[12];
	y = xx * mat[1] + yy * mat[5] + zz * mat[9] + 1.0f * mat[13];
	z = xx * mat[2] + yy * mat[6] + zz * mat[10] + 1.0f * mat[14];

	if ( w != 0.0f )
	{
		float invw = 1.0f / w;
		x *= invw;
		y *= invw;
		z *= invw;
	}

	return *this;
}