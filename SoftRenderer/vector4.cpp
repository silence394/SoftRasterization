#include "vector4.h"
#include "color.h"
#include "matrix4.h"

Vector4::operator Color( ) const
{
	return Color( x, y, z, w );
}

Vector4& Vector4::operator *= ( const Matrix4& mat )
{
	Vector4 v = *this;
	x = v.x * mat[0] + v.y * mat[4] + v.z * mat[8] + v.w * mat[12];
	y = v.x * mat[1] + v.y * mat[5] + v.z * mat[9] + v.w * mat[13];
	z = v.x * mat[2] + v.y * mat[6] + v.z * mat[10] + v.w * mat[14];
	w = v.x * mat[3] + v.y * mat[7] + v.z * mat[11] + v.w * mat[15];

	return *this;
}